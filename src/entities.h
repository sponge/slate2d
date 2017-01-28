#pragma once

#include <entityx/entityx.h>
#include <tmx.h>

void SpawnEntitiesFromLayer(tmx_layer &layer, entityx::EntityManager &es);
bool SpawnEntity(const char *type, entityx::EntityManager &es, tmx_object &obj);