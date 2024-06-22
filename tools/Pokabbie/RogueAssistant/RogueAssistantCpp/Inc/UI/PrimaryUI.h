#pragma once
#include "Timer.h"

struct AssetCollection;

class HomeBoxBehaviour;
class MultiplayerBehaviour;
class Window;

class PrimaryUI
{
public:
	PrimaryUI();
	~PrimaryUI();

	void Render(Window& window);

private:
	enum class PageUI
	{
		Awaiting,
		Multiplayer,
		HomeBox,
	};

	void RenderAwaitingPage(Window& window);
	void RenderMultiplayerPage(Window& window, MultiplayerBehaviour* multiplayer, bool initialLoad);
	void RenderHomeBoxPage(Window& window, HomeBoxBehaviour* homebox, bool initialLoad);

	AssetCollection* m_Assets;

	int m_CurrentConnectionIdx;
	TimeDurationNS m_LastDrawTime;
	PageUI m_CurrentPage;
};