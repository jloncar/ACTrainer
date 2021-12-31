#pragma once
// Lower level feature functions, naked functions etc.

uint32_t asm_localPlayerAddress;
uint32_t asm_jmpBkInvulnerableSelf;

// REPLACES:
// "playerHit": 
// ac_client.exe + 269F0
// 55 8B EC 83 E4 F8
// .text:004269F0                 push    ebp
// .text:004269F1                 mov     ebp, esp
// .text:004269F3				  and esp, 0FFFFFFF8h
// int __usercall playerHit@<eax>(int target@<eax>, int damage, int *attacker, int a4, char a5, char a6)
void __declspec(naked) InvulnerableSelfHook()
{
	__asm {
		// Check if target is local player; if it is - don't take dmg.
		cmp eax, asm_localPlayerAddress
		jne TAKE_DMG
		retn

		TAKE_DMG :
		// Overwritten bytes (6)
		push ebp
			mov ebp, esp
			and esp, 0FFFFFFF8h
			// Jump back, absolute jump 0xFF
			jmp asm_jmpBkInvulnerableSelf
	}
}


bool PatchInvulnerableEveryone(bool enabled)
{
	const size_t sequenceSize = 3;
	byte sequence[sequenceSize] = { 0x29, 0x7B, 0x04 };
	byte newSequence[sequenceSize] = { 0x90, 0x90, 0x90 };

	static uintptr_t sequencePtr = (sequencePtr == NULL) ? Memory::FindSequence(L"ac_client.exe", sequence, sequenceSize) : sequencePtr;

	if (enabled)
	{
		Memory::PatchSequence(sequencePtr, newSequence, sequenceSize);
	}
	else {
		Memory::PatchSequence(sequencePtr, sequence, sequenceSize);
	}

	return true;
}


bool PatchInvulnerableSelf(bool enabled, Game::Engine* game)
{
	// Size of bytes of instruction set we're overwriting.
	// JMP requires minimum of 5 bytes
	const size_t sequenceSize = 6;

	byte originalSequence[sequenceSize] = { 0x55, 0x8B, 0xEC, 0x83, 0xE4, 0xF8 };

	// Replace with pattern scan if needed
	uintptr_t start = game->baseAddress + 0x269F0;

	// Configure vars used in ASM
	asm_localPlayerAddress = (uint32_t)game->player;

	// Resume take dmg addy
	asm_jmpBkInvulnerableSelf = game->baseAddress + 0x269F0 + sequenceSize;

	if (enabled)
	{
		Memory::PatchJump(start, InvulnerableSelfHook, sequenceSize);
	}
	else {
		Memory::PatchSequence(start, originalSequence, sequenceSize);
	}

	return true;
}

void AddGranade(Game::Engine* game)
{
	// (p_baseModule + 0x466B0)

	/// int __usercall addAmmo@<eax>(int playerPtr@<eax>, int a2@<ecx>)

	//int(*addAmmo)(int, int) = (int (*)(int,int))(p_baseModule + 0x466B0);	
	//int addAmmoRes = addAmmo(p_localPlayer + 0xF4, 0x5);

	DWORD playerPtr = (uint32_t)game->player + 0xF4;
	unsigned int ammoType = 0x5;
	DWORD addAmmoAddress = game->baseAddress + 0x466B0;

	// Has to be called from asm because __usercall
	__asm {
		mov eax, playerPtr
		mov ecx, ammoType
		call addAmmoAddress
	}

}


// It's difficult to find matrix, gotta calculate
// https://guidedhacking.com/threads/calculate-your-own-viewprojection-matrix.13597/
// This video solves it for AC, but it's tricky in other games.
// https://guidedhacking.com/threads/how-to-get-started-with-learning-viewmatrix.13663/
// Cameras theory:
// https://www.youtube.com/watch?v=LfbqtmqxX04
// - Cameras don't exist, props of objects in world are changing to adjust for camera change
// - Eg 2D triangle, if "zoomed out" on camera - scale changes.
// - Moving camera left = moving object right actually.
// - This transform is done by using View Matrix.
// - View matrix is position of camera & (pitch, yaw, roll) rotation, used to transformation all objects
// - yaw, roll, pitch: https://guidedhacking.com/threads/how-to-find-view-angles-with-cheat-engine.13994/
// - Projection/Perspective matrix: used to handle camera's FOV and Aspect ratio
// - Objects further away are smaller
// - There's "vanishing point"
// - Perspective: objects in distance are smaller, ortographic: they're same scale
// ----------
// - Projection Matrix / Perspective (FOV & Aspect ratio of camera; near & far clipping points)
//		- glm::perspective -> returns Mat4
// - View matrix (camera position): CameraPosition(x,y,z) , CameraRotation(pitch, yaw, roll)
//		- It's also stored in Mat4, inverse of camera's transform (position&rotation)
// - Model matrix / Transformation matrix (Translation Rotation Scale):
//		- What to apply to models to get desired effect -- ViewMatrix * -1 ?
// - Vertex Matrix - (multiplied in shader eventually for rendering, don't care)
// - World to Screen: ProjectionMatrix x ViewMatrix x Model?
//						ProjectionViewModel (OpenGL)
//						ModelViewProjection (DirectX)
// -----------
// - WorldToScreen matrix multiplication depends on graphics library.
// - OpenGL is column major, DirectX is row major (ie. how x/y are stored in matrix - row or column)
// -----------
// http://songho.ca/opengl/gl_transform.html#matrix
// http://songho.ca/math/homogeneous/homogeneous.html
// https://guidedhacking.com/threads/so-what-is-a-viewmatrix-anyway-and-how-does-a-w2s-work.10964/
// https://www.3dgep.com/understanding-the-view-matrix/
// https://gamedev.stackexchange.com/questions/178643/the-view-matrix-finally-explained
// http://glprogramming.com/red/appendixf.html
// !! https://learnopengl.com/Getting-started/Coordinate-Systems
// !! http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
bool WorldToScreen(Game::Player* player, Game::Player* target, glm::vec2& result)
{
	unsigned int windowWidth = 1024;
	unsigned int windowHeight = 768;

	glm::vec4 targetPosition = glm::vec4(target->positionHead.x, target->positionHead.y, target->positionHead.z, 1.0f);

	// Resolve from like player object?
	glm::vec3 cameraPosition = glm::vec3(player->positionHead.x, player->positionHead.y, player->positionHead.z);
	float cameraPitch = glm::radians(player->viewAngles.y);
	float cameraYaw = glm::radians(player->viewAngles.x);
	float cameraRoll = glm::radians(player->viewAngles.z);


	glm::mat4 Projection = glm::perspective(45.0f, (float)(windowWidth / windowHeight), 1.0f, 100.0f);

	// Build view matrix (camera position, camera rotation)
	glm::mat4 translate = glm::translate(glm::mat4(), cameraPosition);
	glm::mat4 rotate_x = glm::rotate(translate, cameraPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 rotate_y = glm::rotate(translate, cameraYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotate_z = glm::rotate(translate, cameraRoll, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 model = translate * rotate_z * rotate_y * rotate_x;
	glm::mat4 View = glm::inverse(model);
	
	glm::mat4 ViewProjection = Projection * View;

	// Resolve screen coordinates
	glm::vec4 screenVector = targetPosition * ViewProjection;

	float scalingFactor = screenVector.w;

	if (scalingFactor <= 0.0f)
	{
		return false;
	}

	result = glm::vec2(	((screenVector.x / scalingFactor) + 1) * 0.5f * windowWidth,
						((screenVector.y / scalingFactor) + 1) * 0.5f * windowHeight	);

	return true;
}

void DrawESPBox(Game::Player* player, Game::Player* target)
{
	glm::vec2 screenCoords;

	bool w2s = WorldToScreen(player, target, screenCoords);

	if (!w2s)
	{
		return;
	}

	//ImGui::GetForegroundDrawList()->AddCircle(ImVec2(100, 100), 100 * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);
	ImGui::GetForegroundDrawList()->AddRect(ImVec2(screenCoords.x, screenCoords.y),
		ImVec2(screenCoords.x + 100, screenCoords.y + 100),
		IM_COL32(0, 255, 0, 200), 0, 0, 3);
}