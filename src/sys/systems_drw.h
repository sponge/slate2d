#pragma once
#include <entityx/entityx.h>
#include "../local.h"

namespace ex = entityx;

struct RectDrawSystem : public ex::System<RectDrawSystem> {
  explicit RectDrawSystem(ClientInfo *inf) : inf(inf) {};
  void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;
  ClientInfo *inf;
};