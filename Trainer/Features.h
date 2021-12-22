#pragma once
#include "Trainer.h"

// High level exported Features, ready to be used with AddFeature method.
void FreezeAmmo(const Trainer* t, Feature* feature, Game::Engine* game);
void FreezeHealth(const Trainer* t, Feature* feature, Game::Engine* game);
void InvulnerableEveryone(const Trainer* t, Feature* feature, Game::Engine* game);
void InvulnerableSelf(const Trainer* t, Feature* feature, Game::Engine* game);
void ESP(const Trainer* t, Feature* feature, Game::Engine* game);