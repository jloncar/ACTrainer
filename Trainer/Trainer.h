#pragma once
class Trainer
{
private:
	bool b_infiniteHealth;
	int cnt;
public:
		Trainer(HMODULE hModule);
		bool Tick();
};

