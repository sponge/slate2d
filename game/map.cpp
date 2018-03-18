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

tmx_object *Map_LayerObjects(tmx_map *map, int id, tmx_object *current) {
	if (current == nullptr) {
		int i = 0;
		tmx_layer *layer = map->ly_head;
		while (layer != nullptr && i < id) {
			layer = layer->next;
			i++;
		}

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