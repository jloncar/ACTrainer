#include "Features.h"
#include "FeaturesLL.h"

void FreezeAmmo(const Trainer* t, Feature* feature, Game::Engine* game)
{
    if (feature->isEnabled)
    {
        game->player->currentWeapon->ammo->current = 69;
    }
}

void FreezeHealth(const Trainer* t, Feature* feature, Game::Engine* game)
{
    if (feature->isEnabled)
    {
        game->player->health = 420;
    }
}

void InvulnerableEveryone(const Trainer* t, Feature* feature, Game::Engine* game)
{
    if (feature->isEnabled && !feature->isPatched)
    {
        PatchInvulnerableEveryone(true);
        feature->isPatched = true;
    }
    if (!feature->isEnabled && feature->isPatched)
    {
        PatchInvulnerableEveryone(false);
        feature->isPatched = false;
    }
}

void InvulnerableSelf(const Trainer* t, Feature* feature, Game::Engine* game)
{
    if (feature->isEnabled && !feature->isPatched)
    {
        PatchInvulnerableSelf(true, game);
        feature->isPatched = true;
    }
    if (!feature->isEnabled && feature->isPatched)
    {
        PatchInvulnerableSelf(false, game);
        feature->isPatched = false;
    }
}


void ESP(const Trainer* t, Feature* feature, Game::Engine* game)
{
   if(feature->isEnabled)
       DrawESP();
}