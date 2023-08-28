#pragma once
#include <functional>

class GameConnection;

class IGameConnectionTask
{
public:
	virtual void Run(GameConnection& run) = 0;

	//virtual bool IsReadyToRun(GameConnection& run) = 0;
	//
	//virtual bool IsError(GameConnection& run) = 0;
};

class GameConnectionTask
{
public:
	typedef std::function<void(GameConnection&)> GameConnectionMethod;

	GameConnectionTask(GameConnectionMethod method);

private:
	GameConnectionMethod m_Method;
};