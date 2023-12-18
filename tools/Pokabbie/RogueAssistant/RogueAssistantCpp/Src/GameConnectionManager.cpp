#include "GameConnectionManager.h"
#include "GameConnection.h"

#include "Log.h"

//#include <WinSock2.h>

#include <SFML/Network.hpp>

GameConnectionManager& GameConnectionManager::Instance()
{
	static GameConnectionManager manager;
	return manager;
}

void GameConnectionManager::OpenListener()
{
	LOG_INFO("Game: Opening connection listener");
	ASSERT_MSG(m_Listener == NULL, "Listener already active");
	m_Listener = std::make_unique<sf::TcpListener>();
	m_Listener->setBlocking(false);

	if (m_Listener->listen(GameConnectionManager::c_DefaultPort) != sf::Socket::Done)
	{
		ASSERT_FAIL("Game: Failed to open connection listener");
		return;
	}
}

void GameConnectionManager::CloseListener()
{
	LOG_INFO("Game: Closing connection listener");
	ASSERT_MSG(m_Listener != NULL, "Listener not active");
	m_Listener->close();
	m_Listener = nullptr;
}

void GameConnectionManager::UpdateConnections()
{
	// Accept any incoming connections
	if (m_AcceptingConnection == nullptr)
		m_AcceptingConnection = std::make_shared<GameConnection>();

	if (m_Listener->accept(m_AcceptingConnection->m_Socket) == sf::Socket::Done)
	{
		LOG_INFO("Game: Incoming connection...");

		GameConnectionRef gameConn = m_AcceptingConnection;
		m_AcceptingConnection = nullptr;

		ActiveGameConnection newConnection;
		newConnection.m_Game = gameConn;
		newConnection.m_UpdateThread = std::thread([this, gameConn]() { BackgroundUpdate(gameConn); });
		m_ActiveConnections.push_back(std::move(newConnection));
	}

	// Handle disconnections
	for (int i = 0; i < (int)m_ActiveConnections.size();)
	{
		if (m_ActiveConnections[i].m_Game->HasDisconnected())
		{
			LOG_INFO("Game: Connection disconnected");
			m_ActiveConnections[i].m_UpdateThread.join();
			m_ActiveConnections.erase(m_ActiveConnections.begin() + i);
		}
		else
			++i;
	}
}

void GameConnectionManager::BackgroundUpdate(GameConnectionRef game)
{
	// At max run at 30UPS for now
	while (!game->HasDisconnected())
	{
		game->Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 30));
	}
}