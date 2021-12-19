#include "Features.h"


void FreezeAmmo(const Trainer* t, Feature* feature, Game* game)
{
    if (feature->isEnabled)
    {
        *game->localPlayer->ammo = 69;
    }
}

void FreezeHealth(const Trainer* t, Feature* feature, Game* game)
{
    if (feature->isEnabled)
    {
        *game->localPlayer->hp = 420;
    }
}

void InvulnerableEveryone(const Trainer* t, Feature* feature, Game* game)
{
    if (feature->isEnabled && !feature->isPatched)
    {
        game->PatchInvulnerableEveryone(true);
        feature->isPatched = true;
    }
    if (!feature->isEnabled && feature->isPatched)
    {
        game->PatchInvulnerableEveryone(false);
        feature->isPatched = false;
    }
}

void InvulnerableSelf(const Trainer* t, Feature* feature, Game* game)
{
    if (feature->isEnabled && !feature->isPatched)
    {
        game->PatchInvulnerableSelf(true);
        feature->isPatched = true;
    }
    if (!feature->isEnabled && feature->isPatched)
    {
        game->PatchInvulnerableSelf(false);
        feature->isPatched = false;
    }
}



void DrawESP()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoInputs;

    ImGui::Begin("ESP", nullptr, window_flags);

    ImGui::GetForegroundDrawList()->AddCircle(ImVec2(100, 100), 100 * 0.6f, IM_COL32(0, 255, 0, 200), 0, 10);

    ImGui::End();
}

void ESP(const Trainer* t, Feature* feature, Game* game)
{
   if(feature->isEnabled)
       DrawESP();
}