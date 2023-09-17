
#include "enet/enet.h"

#include "Log.h"
#include <WinSock2.h>

void GameTest();

void TestEntry()
{
	if (enet_initialize() != 0)
	{
		ASSERT_FAIL("ENet: Failed to initialise");
		return;
	}

	ENetAddress address;
	ENetHost* server;

	address.host = ENET_HOST_ANY;
	address.port = 20125;

	server = enet_host_create(&address,
		32, // client count
		2,  // channel count
		0,  // assumed incoming bandwidth
		0   // assumed outgoing bandwidth
	);

	if (server == nullptr)
	{
		ASSERT_FAIL("ENet: Failed to create host");
		return;
	}


	enet_host_destroy(server);

	// At exit
	enet_deinitialize();
	return;
}