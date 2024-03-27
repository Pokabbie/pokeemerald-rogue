#pragma once
#include "Defines.h"
#include "SFML/Network.hpp"
#include <memory>
#include <thread>

class GameConnection;
class GameConnectionManager;

typedef std::shared_ptr<GameConnection> GameConnectionRef;

struct ActiveGameConnection
{
	GameConnectionRef m_Game;
	std::thread m_UpdateThread;
};

class GameConnectionManager
{
public:
	static int const c_DefaultPort = 30125;

	static GameConnectionManager& Instance();

	void OpenListener();
	void CloseListener();

	void UpdateConnections();

	inline bool AnyConnectionsActive() const { return !m_ActiveConnections.empty(); }
	inline int ActiveConnectionCount() const { return m_ActiveConnections.size(); }

	inline ActiveGameConnection& GetGameConnectionAt(int index) { return m_ActiveConnections[index]; }

private:
	GameConnectionManager() = default;
	void BackgroundUpdate(GameConnectionRef game);

	std::unique_ptr<sf::TcpListener> m_Listener;

	std::vector<ActiveGameConnection> m_ActiveConnections;
	GameConnectionRef m_AcceptingConnection;
};