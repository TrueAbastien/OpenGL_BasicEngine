#pragma once

#include "CollisionManager.hpp"

class CollisionDetector final : public Component
{
public:
  CollisionDetector(CollisionManager* manager);

protected:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override;

private:
  CollisionManager* m_manager;
};