#pragma once
#include <functional>
#include <string>

class Window;
struct GLFWwindow;

typedef std::function<bool(Window*, void*)> WindowCallback;

struct WindowConfig
{
	std::string title;
	bool imGuiEnabled = false;
};

class Window
{
public:
	Window(WindowConfig const&);

	bool Create();
	bool Destroy();

	void EnterMainLoop(WindowCallback callback, void* userData = nullptr);

private:
	WindowConfig m_Config;
	GLFWwindow* m_GlfwWindow;
};