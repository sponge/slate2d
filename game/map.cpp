#include <tmx.h>
#include <cstring>

int Map_GetLayerByName(tmx_map *map, const char *name) {
	int i = 0;
	tmx_layer *layer = map->ly_head;
	while (layer != nullptr) {
		if (strcmp(name, layer->name) == 0) {
			return i;
		}
		i++;
		layer = layer->next;
	}

	return -1;
}

tmx_layer *Map_GetLayer(tmx_map *map, int id) {
	int i = 0;
	tmx_layer *layer = map->ly_head;
	while (layer != nullptr && i < id) {
		layer = layer->next;
		i++;
	}

	return layer;
}

tmx_object *Map_LayerObjects(tmx_map *map, int id, tmx_object *current) {
	if (id < 0) {
		return nullptr;
	}

	if (current == nullptr) {
		tmx_layer *layer = Map_GetLayer(map, id);

		if (layer->type != L_OBJGR) {
			return nullptr;
		}

		return layer->content.objgr->head;
	}
	else {
		return current->next;
	}
}

const char *Map_GetObjectType(tmx_map *map, tmx_object *obj) {
	if (obj->type) {
		return obj->type;
	}

	if (obj->content.gid) {
		return map->tiles[obj->content.gid]->type;
	}

	return nullptr;
}

tmx_tile *Map_GetTileInfo(tmx_map *map, unsigned int gid) {
	if (gid > map->tilecount) {
		return nullptr;
	}

	return map->tiles[gid];
}

unsigned int Map_GetTile(tmx_map *map, int id, unsigned int x, unsigned int y) {
	tmx_layer *layer = Map_GetLayer(map, id);
	if (layer->type != L_LAYER) {
		return 0;
	}
	
	return (layer->content.gids[(y*map->width) + x]) & TMX_FLIP_BITS_REMOVAL;
}