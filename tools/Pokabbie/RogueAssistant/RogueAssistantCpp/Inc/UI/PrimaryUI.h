#pragma once
#include "Timer.h"

struct AssetCollection;

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
	};

	void RenderAwaitingPage(Window& window);
	void RenderMultiplayerPage(Window& window, MultiplayerBehaviour* multiplayer, bool initialLoad);

	AssetCollection* m_Assets;

	int m_CurrentConnectionIdx;
	TimeDurationNS m_LastDrawTime;
	PageUI m_CurrentPage;
};