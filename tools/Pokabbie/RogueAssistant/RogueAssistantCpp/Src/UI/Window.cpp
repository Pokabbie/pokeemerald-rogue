#include "UI\Window.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

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
        sf::Event sfEvent;
        while (m_WindowHandle->pollEvent(sfEvent))
        {
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
