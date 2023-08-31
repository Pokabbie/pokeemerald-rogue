#pragma once
#include "GameConnectionBehaviour.h"

class MultiplayerBehaviour : public IGameConnectionBehaviour
{
public:
	virtual void OnAttach(GameConnection& game) override;
	virtual void OnDetach(GameConnection& game) override;
	virtual void OnUpdate(GameConnection& game) override;
};