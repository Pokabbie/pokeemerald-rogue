#pragma once
#include <functional>
#include <bitset>
#include <string>
#include <SFML/Window.hpp>

class Window;

namespace sf
{
	class RenderWindow;
}

typedef std::function<bool(Window*, void*)> WindowCallback;

struct WindowConfig
{
	std::string title;
	int width = 640;
	int height = 480;
	bool resizable = false;
	bool imGuiEnabled = false;
};

class Window
{
public:
	Window(WindowConfig const&);

	bool Create();
	bool Destroy();

	void EnterMainLoop(WindowCallback callback, void* userData = nullptr);

	inline sf::RenderWindow* GetHandle() { return m_WindowHandle; }
	inline sf::RenderWindow const * GetHandle() const { return m_WindowHandle; }

	inline bool IsButtonHeld(sf::Keyboard::Key key) const { return m_CurrentKeyStates.test(key); }
	inline bool ButtonJustPressed(sf::Keyboard::Key key) const { return m_CurrentKeyStates.test(key) && !m_PreviousKeyStates.test(key); }
	inline bool ButtonJustReleased(sf::Keyboard::Key key) const { return !m_CurrentKeyStates.test(key) && m_PreviousKeyStates.test(key); }

	inline void ClearInputText() { m_TextEntered = ""; }
	inline void SetInputText(std::string const& text) { m_TextEntered = text; }
	inline std::string const& GetInputText() const { return m_TextEntered; }

private:
	WindowConfig m_Config;
	sf::RenderWindow* m_WindowHandle;

	std::string m_TextEntered;
	std::bitset<sf::Keyboard::KeyCount> m_CurrentKeyStates;
	std::bitset<sf::Keyboard::KeyCount> m_PreviousKeyStates;
};