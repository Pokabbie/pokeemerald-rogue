#include "UI\PrimaryUI.h"
#include "UI\Window.h"
#include "Assets.h"
#include "GameConnectionManager.h"
#include "Log.h"

#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Windows.h>

#pragma warning(disable: 4244)

int RogueAssistant_Main(std::vector<std::string> const& args);
bool RogueAssistant_MainLoop(Window* window, void* userData);

#ifdef _DEBUG
int main(int argc, const char** argv)
{
    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i)
    {
        args.push_back(std::string(argv[i]));
    }

    return RogueAssistant_Main(args);
}

#else

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    std::vector<std::string> args;

    for (int i = 0; i < __argc; ++i)
    {
        std::string str(__argv[i]);
        args.push_back(str);
    }

    return RogueAssistant_Main(args);
}

#endif

static void DumpScriptsNextToExe()
{
    std::ofstream fileStream;
    fileStream.open("RogueAssistant_mGBA.lua", std::ios::out);

    LOG_INFO("Dumping RogueAssistant_mGBA.lua next to exe");

    auto const& data = bin2cpp::getRogueAssistant_mGBALuaFile();
    char const* ptr = data.getBuffer();

    for (;*ptr != 0; ++ptr)
    {
        // Ignore carriage return
        if (*ptr != '\r')
            fileStream << *ptr;
    }

    fileStream.close();
}

int RogueAssistant_Main(std::vector<std::string> const& args)
{
    DumpScriptsNextToExe();

    WindowConfig config;
    config.title = "Rogue Assistant";
    config.imGuiEnabled = false;

    Window window(config);
    PrimaryUI ui;

    if (window.Create())
    {
        GameConnectionManager::Instance().OpenListener();

        window.EnterMainLoop(RogueAssistant_MainLoop, &ui);
        if (window.Destroy())
        {
            GameConnectionManager::Instance().CloseListener();
            return 0;
        }
    }

    return 1;
}

bool RogueAssistant_MainLoop(Window* window, void* userData)
{
    PrimaryUI* ui = (PrimaryUI*)userData;
    GameConnectionManager::Instance().UpdateConnections();
    ui->Render(*window);
    return true;
}