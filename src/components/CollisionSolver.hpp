#pragma once

#include "CollisionManager.hpp"

class CollisionSolver final : public Component
{
public:
  CollisionSolver(CollisionManager* manager);

protected:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  CollisionManager* m_manager;
};