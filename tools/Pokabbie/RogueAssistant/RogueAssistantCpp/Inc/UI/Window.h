#pragma once
#include <functional>
#include <string>

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

private:
	WindowConfig m_Config;
	sf::RenderWindow* m_WindowHandle;
};