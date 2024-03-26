#pragma once
#include "Timer.h"

struct AssetCollection;

class Window;

class PrimaryUI
{
public:
	PrimaryUI();
	~PrimaryUI();

	void Render(Window& window);

private:
	AssetCollection* m_Assets;

	TimeDurationNS m_LastDrawTime;
};