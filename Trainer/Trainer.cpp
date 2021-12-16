#include "Trainer.h"

Trainer::Trainer(HMODULE& hModule, const wchar_t* gameWindowTitle)
{
    m_Config = new Config();
    m_Config->b_invulnerableEveryone = false;
    m_Config->b_infiniteAmmo = false;
    m_Config->b_noRecoil = false;
    m_Config->b_aimbot = false;

    m_GameWindow = FindWindow(NULL, gameWindowTitle);

    m_Overlay = new Overlay();
    m_Overlay->Init(m_Config, hModule, m_GameWindow);


}


bool Trainer::Tick() {
    LocalPlayer player;

    static bool b_invulnerableEveryonePatched, b_invulnerableSelfPatched;

    int messageCode;

    if (m_Config->b_invulnerableEveryone && !b_invulnerableEveryonePatched)
    {
        player.PatchInvulnerableEveryone(true);
        b_invulnerableEveryonePatched = true;
    }
    if (!m_Config->b_invulnerableEveryone && b_invulnerableEveryonePatched)
    {
        player.PatchInvulnerableEveryone(false);
        b_invulnerableEveryonePatched = false;
    }


    if (m_Config->b_invulnerableSelf && !b_invulnerableSelfPatched)
    {
        player.PatchInvulnerableSelf(true);
        b_invulnerableSelfPatched = true;
    }
    if (!m_Config->b_invulnerableSelf && b_invulnerableSelfPatched)
    {
        player.PatchInvulnerableSelf(false);
        b_invulnerableSelfPatched = false;
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


    if (!m_Overlay->Tick(messageCode))
    {
        // UI Error
        return false;
    }

    // Process message
    switch (messageCode)
    {
    case TRAINER_UI_MSG::ADD_GRANADE:
        player.AddGranade();
        break;
    }
    messageCode = 0;

    return true;
}

Trainer::~Trainer()
{
    delete m_Config;
    delete m_Overlay;
}