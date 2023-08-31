#pragma once
#include <memory>

class GameConnection;

class IGameConnectionBehaviour : public std::enable_shared_from_this<IGameConnectionBehaviour>
{
public:
	virtual ~IGameConnectionBehaviour() = default;

	virtual void OnAttach(GameConnection& game) = 0;
	virtual void OnDetach(GameConnection& game) = 0;
	virtual void OnUpdate(GameConnection& game) = 0;
};

typedef std::shared_ptr<IGameConnectionBehaviour> GameConnectionBehaviourRef;