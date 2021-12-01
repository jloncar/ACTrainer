#include "Trainer.h"

Trainer::Trainer(HMODULE& hModule)
{
    m_Config = new Config();
    m_Config->b_invulnerable = false;
    m_Config->b_infiniteAmmo = false;
    m_Config->b_noRecoil = false;
    m_Config->b_aimbot = false;

    HWND window = FindWindow(NULL, L"AssaultCube");

    m_Overlay = new Overlay();
    m_Overlay->Init(m_Config, hModule, window);
}
	
bool Trainer::Tick() {
    if (!m_Overlay->Tick())
    {
        return false;
    }
    return true;
}

Trainer::~Trainer()
{
    delete m_Config;
    delete m_Overlay;
}