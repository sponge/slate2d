#pragma once
#include "pecs.h"
#include "baseworld.h"

const Sweep Trace(BaseWorld &world, entity_t &ent, float dx, float dy, entity_t *hitEnt);
const entity_t* CheckTrigger(BaseWorld &world, entity_t &ent);