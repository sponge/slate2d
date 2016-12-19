#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"
#include <tmx.h>

inline unsigned int gid_clear_flags(unsigned int gid) {
	return gid & TMX_FLIP_BITS_REMOVAL;
}

void TileMapDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	NVGcontext *nvg = inf->nvg;

	for (auto ent : es.entities_with_components<TileMap>()) {
		auto tileMap = ent.component<TileMap>();

		auto layer = tileMap->map->ly_head;
		while (layer) {
			if (layer->visible == false || layer->type == L_OBJGR) {
				layer = layer->next;
				continue;
			}

			if (layer->type == L_IMAGE) {
				draw_image(inf, tileMap->map, layer);
			}
			else if (layer->type == L_LAYER) {
				draw_tiles(inf, tileMap->map, layer);
			}

			layer = layer->next;
		}
	}
}

void TileMapDrawSystem::draw_image(ClientInfo * inf, tmx_map * map, tmx_layer * layer) {

}

void TileMapDrawSystem::draw_tiles(ClientInfo * inf, tmx_map * map, tmx_layer * layer) {
	for (unsigned int y = 0; y < map->height; y++) {
		for (unsigned int x = 0; x < map->width; x++) {
			auto gid = gid_clear_flags(layer->content.gids[(y*map->width) + x]);
			if (gid == 0) {
				continue;
			}

			auto tile = map->tiles[gid];
			auto ts = tile->tileset;

			int sx = tile->ul_x;
			int sy = tile->ul_y;
			auto w = ts->tile_width;
			auto h = ts->tile_height;
			int dx = x*ts->tile_width;
			int dy = y*ts->tile_height;

			Img *img = (Img*)tile->tileset->image->resource_image;

			auto paint = nvgImagePattern(img->nvg, dx - sx, dy - sy, img->w, img->h, 0, img->hnd, 1.0f);
			nvgBeginPath(img->nvg);
			nvgRect(img->nvg, dx, dy, w, h);
			nvgFillPaint(img->nvg, paint);
			nvgFill(img->nvg);

			//nvgStrokeColor(img->nvg, nvgRGBA(255, 0, 0, 255));
			//nvgStrokeWidth(img->nvg, 1);
			//nvgStroke(img->nvg);
			continue;
		}
	}
}
