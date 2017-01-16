#include <entityx/entityx.h>
#include "local.h"
#include "components.h"
#include "console/console.h"

namespace ex = entityx;

void SP_player(ex::EntityManager &es, tmx_object &obj) {
    // get the reference to the main world entity
    auto iter = es.entities_with_components<TileMap>();
    ex::Entity worldEnt = *(iter.begin());
    auto tmap = worldEnt.component<TileMap>();

    // create the player
    auto ent = es.create();
    auto body = ent.assign<Body>(obj.x + (tmap->map->tile_width / 2), obj.y - 14.001, 14, 28);  // FIXME able to fall into ground if 14
    ent.assign<Movable>(0, 0);
    ent.assign<Renderable>(200, 30, 30, 255);
    ent.assign<PlayerInput>();

    // attach a camera to the world and target it at the player
    auto camera = worldEnt.assign<Camera>(1280, 720, 3, tmap->map->width * tmap->map->tile_width, tmap->map->height * tmap->    map->tile_height);
    camera->target = body.get();
}

typedef struct {
	char	*name;
	void	(*spawn)(ex::EntityManager &em, tmx_object &obj);
} spawn_t;

spawn_t	spawns[] = {
    {"player", SP_player}
};

bool SpawnEntitiesFromLayer(tmx_layer &layer, ex::EntityManager &es) {
    tmx_object *obj = layer.content.objgr->head;
    while (obj != nullptr) {
        if (obj->type == nullptr) {
            obj = obj->next;
            continue;
        }

        SpawnEntity(obj->type, es, *obj);
        obj = obj->next;
    }
}

bool SpawnEntity(const char *type, ex::EntityManager &es, tmx_object &obj) {
    spawn_t	*s;

	for ( s=spawns ; s->name ; s++ ) {
		if ( !strcmp(s->name, type) ) {
			s->spawn(es, obj);
			return true;
		}
	}

	Com_Printf("%s doesn't have a spawn function\n", type);
    return false;
}