#pragma once
#include <tmx.h>

int Map_GetLayerByName(tmx_map *map, const char *name);
tmx_layer *Map_GetLayer(tmx_map *map, int id);
tmx_object *Map_LayerObjects(tmx_map *map, int layer, tmx_object *current);
const char *Map_GetObjectType(tmx_map *map, tmx_object *obj);
tmx_tile *Map_GetTileInfo(tmx_map *map, unsigned int gid);
unsigned int Map_GetTile(tmx_map *map, int id, unsigned int x, unsigned int y);