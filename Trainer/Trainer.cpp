#include "Trainer.h"

Trainer::Trainer(HMODULE& hModule, const wchar_t* gameWindowTitle)
{
    m_Config = new Config();
    m_Config->b_invulnerable = false;
    m_Config->b_infiniteAmmo = false;
    m_Config->b_noRecoil = false;
    m_Config->b_aimbot = false;

    m_GameWindow = FindWindow(NULL, gameWindowTitle);

    m_Overlay = new Overlay();
    m_Overlay->Init(m_Config, hModule, m_GameWindow);

}


bool Trainer::Tick() {
    LocalPlayer player;

    if (!m_Overlay->Tick())
    {
        return false;
    }


    if (m_Config->b_freezeHp)
    {
        // Write to health
        (*player.hp) = 420;
    }

    if (m_Config->b_infiniteAmmo)
    {
        // Write to health
        (*player.ammo) = 69;
    }

    return true;
}

Trainer::~Trainer()
{
    delete m_Config;
    delete m_Overlay;
}