// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


/*
                  I claim very little to no ownership of this code.
    99% of the codebase is copy pasted from IW6x, i simply vaguely "re-wrote" it to work with steam IW6.

    Check out the IW6x project on their github : https://github.com/XLabsProject/iw6x-client
                             (C) XLabs - IW6x
*/


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        switch (MessageBox(NULL, L"Using this tool may result in a ban.\nBy pressing no, the DLL will attempt to unload.\nDo you want to continue?", L"ujicos' Ghosts Unlock All", MB_ICONINFORMATION | MB_YESNO))
        {
        case IDYES:
            AllocConsole();
            freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
            freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
            SetConsoleTitleA("ujicos' Ghosts Unlock All");
            system("cls");

            printf("Starting unlocks, please do not close this window.\n");

            unlock_all_challenges();
            unlock_all_multiplayer();
            unlock_all_extinction();
            unlock_past_title_backgrounds();

            keyBinds();

            //Unlocks DLC Items
            printf("[DLC] Unlocking DLC & Developer items...\n");
            hook::set(0x1404EAC50, 0xC301B0); // IW6x - Content_DoWeHaveDLCPackByName
            hook::set(0x140599890, 0xC301B0); // IW6x - Entitlements_IsIDUnlocked


            printf("\n\n\nAll done! Do not close the console window, as this will cause the game to exit.");
            break;
        case IDNO:
            //This doesnt seem to actually unload the DLL?
            ExitThread(0);
            break;
        default:
            /* An error occurred. */
            break;
        }
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

