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

bool _WorldToScreen(glm::vec3 pos, glm::vec3& screen, float matrix[16], int windowWidth, int windowHeight)
{
	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	glm::vec4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	//perspective division, dividing by clip.W = Normalized Device Coordinates
	glm::vec3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	//Transform to window coordinates
	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}




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

bool WorldToScreen(Game::Player* player, Game::Player* target, glm::vec3& screenVector, const int windowWidth, const int windowHeight)
{
	glm::vec3 playerPosition = glm::vec3(player->positionBody.x, player->positionBody.y, player->positionBody.z);
	glm::vec3 targetPosition = glm::vec3(target->positionHead.x, target->positionHead.y, target->positionHead.z);

	float* p_projectionMatrix = reinterpret_cast<float*>(0x501AE8);
	
	return _WorldToScreen(targetPosition, screenVector, p_projectionMatrix, windowWidth, windowHeight);
}


void ResolveBoundingRectangleCoords(const glm::vec3& screenCoords, const float distance, const int windowWidth, const int windowHeight, ImVec2& start, ImVec2& end)
{
	const float playerModelAspectRation = 2.5;
	const float boxSize = 500;
	const float scaler = (boxSize / distance) * (windowWidth/768);

	start	= ImVec2(screenCoords.x - scaler, screenCoords.y - scaler);
	end		= ImVec2(screenCoords.x + scaler * 2, screenCoords.y + scaler * playerModelAspectRation * 2);
}

void DrawESPBox(Game::Player* player, Game::Player* target)
{
	const int windowWidth = 1024;
	const int windowHeight = 768;

	glm::vec3 screenCoords;

	bool w2s = WorldToScreen(player, target, screenCoords, windowWidth, windowHeight);

	if (!w2s)
	{
		return;
	}

	const float distance = glm::abs(glm::distance(	glm::vec3(player->positionBody.x, player->positionBody.y, player->positionBody.z),
													glm::vec3(target->positionBody.x, target->positionBody.y, target->positionBody.z)));

	ImVec2 start;
	ImVec2 end;
	ResolveBoundingRectangleCoords(screenCoords, distance, windowWidth, windowHeight, start, end);

	ImGui::GetForegroundDrawList()->AddRect(start, end, IM_COL32(0, 255, 0, 200), 0, 0, 3);
}