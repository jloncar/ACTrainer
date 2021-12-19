#pragma once
#include "Trainer.h"

void FreezeAmmo(const Trainer* t, Feature* feature, Game* game);
void FreezeHealth(const Trainer* t, Feature* feature, Game* game);
void InvulnerableEveryone(const Trainer* t, Feature* feature, Game* game);
void InvulnerableSelf(const Trainer* t, Feature* feature, Game* game);
void DrawESP();
void ESP(const Trainer* t, Feature* feature, Game* game);