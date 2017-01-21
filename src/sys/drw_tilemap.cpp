#include <entityx/entityx.h>
#include "systems_drw.h"
#include "../components.h"
#include "../glstuff/shaderquad.h"
#include <tmx.h>

inline unsigned int gid_clear_flags(unsigned int gid) {
	return gid & TMX_FLIP_BITS_REMOVAL;
	
}

void TileMapDrawSystem::update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) {
	ex::ComponentHandle<Camera> activeCam;
	for (auto ent : es.entities_with_components<Camera>()) {
		auto cam = ent.component<Camera>();
		if (cam->active) {
			activeCam = cam;
			break;
		}
	}

	for (auto ent : es.entities_with_components<TileMap>()) {
		auto tileMap = ent.component<TileMap>();

		nvgBeginPath(inf->nvg);
		nvgRect(inf->nvg, activeCam->left, activeCam->top, activeCam->size.x / activeCam->scale, activeCam->size.y / activeCam->scale);
		nvgFillColor(inf->nvg, nvgRGBA(
			(tileMap->map->backgroundcolor >> 16) & 0xFF,
			(tileMap->map->backgroundcolor >> 8) & 0xFF,
			(tileMap->map->backgroundcolor) & 0xFF,
			255
		));
		nvgFill(inf->nvg);

		auto layer = tileMap->map->ly_head;
		while (layer) {
			if (layer->visible == false || layer->type == L_OBJGR) {
				layer = layer->next;
				continue;
			}

			if (layer->type == L_IMAGE) {
				draw_image(inf, tileMap->map, layer, activeCam);
			}
			else if (layer->type == L_LAYER) {
				draw_tiles(inf, tileMap->map, layer, activeCam);
			}

			layer = layer->next;
		}
	}
}

void TileMapDrawSystem::draw_image(ClientInfo * inf, tmx_map * map, tmx_layer * layer, ex::ComponentHandle<Camera> &cam) {

}

void TileMapDrawSystem::draw_tiles(ClientInfo * inf, tmx_map * map, tmx_layer * layer, ex::ComponentHandle<Camera> &cam) {
	for (unsigned int y = cam->top / map->tile_height; y < cam->bottom / map->tile_height; y++) {
		for (unsigned int x = cam->left / map->tile_width; x < cam->right / map->tile_width; x++) {
			auto raw = layer->content.gids[(y*map->width) + x];
			auto gid = gid_clear_flags(raw);
			if (gid == 0) {
				continue;
			}

			int flipX = raw & TMX_FLIPPED_HORIZONTALLY ? -1 : 1;
			int flipY = raw & TMX_FLIPPED_VERTICALLY ? -1 : 1;
			int flipDiag = raw & TMX_FLIPPED_DIAGONALLY ? 90 : 0;

			tmx_tile *tile = map->tiles[gid];
			tmx_tileset *ts = tile->tileset;

			int sx = tile->ul_x;
			int sy = tile->ul_y;
			int w = ts->tile_width;
			int h = ts->tile_height;
			int dx = x*ts->tile_width;
			int dy = y*ts->tile_height;

			Img *img = (Img*)tile->tileset->image->resource_image;

			nvgSave(img->nvg);

			nvgTranslate(img->nvg, dx + (w/2), dy + (h/2));
			nvgRotate(img->nvg, nvgDegToRad(flipDiag));
			nvgScale(img->nvg, flipX, flipY);
			auto paint = nvgImagePattern(img->nvg, -(w/2)-sx, -(h/2)-sy, img->w, img->h, 0, img->hnd, 1.0f);
			nvgBeginPath(img->nvg);
			nvgRect(img->nvg, -(w/2), -(h/2), w, h);
			nvgFillPaint(img->nvg, paint);
			//nvgFillColor(img->nvg, nvgRGBA(0, 255, 0, 255));
			nvgFill(img->nvg);

			nvgRestore(img->nvg);
		}
	}
}
