#pragma once
#include "Defines.h"
#include "SFML/Network.hpp"
#include <memory>

class GameConnection;
class GameConnectionManager;

typedef std::shared_ptr<GameConnection> GameConnectionRef;

class GameConnectionManager
{
public:
	static int const c_DefaultPort = 30125;

	static GameConnectionManager& Instance();

	void OpenListener();
	void CloseListener();

	void UpdateConnections();

private:
	GameConnectionManager() = default;

	std::unique_ptr<sf::TcpListener> m_Listener;

	std::vector<GameConnectionRef> m_ActiveConnections;
	GameConnectionRef m_AcceptingConnection;
};