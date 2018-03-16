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