#pragma once
#include "Trainer.h"

void FreezeAmmo(const Trainer* t, Feature* feature, LocalPlayer* player);
void FreezeHealth(const Trainer* t, Feature* feature, LocalPlayer* player);
void InvulnerableEveryone(const Trainer* t, Feature* feature, LocalPlayer* player);
void InvulnerableSelf(const Trainer* t, Feature* feature, LocalPlayer* player);
void ESP(const Trainer* t, Feature* feature, LocalPlayer* player);