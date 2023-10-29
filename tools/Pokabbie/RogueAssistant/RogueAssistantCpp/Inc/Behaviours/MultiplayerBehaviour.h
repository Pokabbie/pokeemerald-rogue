#pragma once
#include "Defines.h"
#include "GameConnectionBehaviour.h"

class MultiplayerBehaviour : public IGameConnectionBehaviour
{
public:
	virtual void OnAttach(GameConnection& game) override;
	virtual void OnDetach(GameConnection& game) override;
	virtual void OnUpdate(GameConnection& game) override;

private:
	void OpenHostConnection(GameConnection& game);
	void OpenClientConnection(GameConnection& game);
	void CloseConnection(GameConnection& game);

	void SendMultiplayerConfirmationToGame(GameConnection& game);

	u8 m_RequestFlags;
};