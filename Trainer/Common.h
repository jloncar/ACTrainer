#pragma once

enum TRAINER_UI_MSG {
	ADD_GRANADE = 1
};

struct Config {
	bool b_freezeHp;
	bool b_invulnerableEveryone;
	bool b_invulnerableSelf;
	bool b_infiniteAmmo;
	bool b_healingGranades;

	bool b_noRecoil;
	bool b_aimbot;
};