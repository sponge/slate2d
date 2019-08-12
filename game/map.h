#pragma once
#include <tmx.h>

int Map_GetLayerByName(const tmx_map *map, const char *name);
tmx_layer *Map_GetLayer(const tmx_map *map, int id);
tmx_object *Map_LayerObjects(const tmx_map *map, int layer, const tmx_object *current);
const char *Map_GetObjectType(const tmx_map *map, const tmx_object *obj);
tmx_tile *Map_GetTileInfo(const tmx_map *map, unsigned int gid);
unsigned int Map_GetTile(const tmx_map *map, int id, unsigned int x, unsigned int y);