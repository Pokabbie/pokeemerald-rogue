#include "UI\Window.h"
#include "Assets.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <Windows.h>

#include "Defines.h"
#include "Log.h"


Window::Window(WindowConfig const& config)
    : m_Config(config)
{
}

bool Window::Create()
{
    LOG_INFO("Creating Window");

    m_WindowHandle = new sf::RenderWindow();
    m_WindowHandle->create(sf::VideoMode(m_Config.width, m_Config.height), m_Config.title);
    m_WindowHandle->setVerticalSyncEnabled(true);

    sf::Image icon;
    icon.loadFromMemory(bin2cpp::getWobbuffetImagePngFile().getBuffer(), bin2cpp::getWobbuffetImagePngFile().getSize());
    m_WindowHandle->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    return true;
}

bool Window::Destroy()
{
    if (m_WindowHandle != nullptr)
    {
        m_WindowHandle->close();
        delete m_WindowHandle;
        m_WindowHandle = nullptr;
    }
    return true;
}

void Window::EnterMainLoop(WindowCallback callback, void* userData)
{
    if (m_WindowHandle == nullptr)
    {
        ASSERT_FAIL("Window not created yet");
        return;
    }

    bool continueLoop = true;

    while (m_WindowHandle->isOpen() && continueLoop)
    {
        m_PreviousKeyStates = m_CurrentKeyStates;

        sf::Event sfEvent;
        while (m_WindowHandle->pollEvent(sfEvent))
        {
            if (sfEvent.type == sf::Event::KeyPressed && sfEvent.key.code != sf::Keyboard::Unknown)
                m_CurrentKeyStates.set(sfEvent.key.code, true);

            if (sfEvent.type == sf::Event::KeyReleased && sfEvent.key.code != sf::Keyboard::Unknown)
                m_CurrentKeyStates.set(sfEvent.key.code, false);

            if (sfEvent.type == sf::Event::TextEntered && sfEvent.text.unicode < 128)
            {
                if (sfEvent.text.unicode == 8) // backspace
                {
                    if (!m_TextEntered.empty())
                        m_TextEntered = m_TextEntered.substr(0, m_TextEntered.size() - 1);
                }
                else if (sfEvent.text.unicode >= 1 && sfEvent.text.unicode <= 26) // ctrl + ?
                {
                    if (sfEvent.text.unicode == 22) // ctrl + v
                    {
                        if (OpenClipboard(NULL))
                        {
                            HANDLE h = GetClipboardData(CF_TEXT);
                            char* textPtr = (char*)h;

                            if (textPtr != nullptr)
                            {
                                // Limit to specific character limit just for ease
                                for (int i = 0; i < 256 && textPtr[i] != 0; ++i)
                                    m_TextEntered += textPtr[i];
                            }

                            CloseClipboard();
                        }
                    }
                    else if (sfEvent.text.unicode == 1) // ctrl + a
                    {
                        // breakig the rules just to make it easy to delete all the text
                        m_TextEntered = "";
                    }
                }
                else
                    m_TextEntered += static_cast<char>(sfEvent.text.unicode);
            }

            // "close requested" event: we close the window
            if (sfEvent.type == sf::Event::Closed)
                continueLoop = false;
        }
                
        m_WindowHandle->clear();

        if(!callback(this, userData))
            continueLoop = false;

        m_WindowHandle->display();
    }

    Destroy();
}
