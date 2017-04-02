#include "systems.h"
#include "../baseworld.h"
#include "../../src/image.h"
#include "../public.h"

TileMapDrawSystem::TileMapDrawSystem(ClientInfo *inf) : inf(inf)
{
	this->name = "Draw TileMap";
	this->priority = 0;
	this->mask = COMPONENT_TILEMAP;
	this->renderOnly = true;
}

inline unsigned int gid_clear_flags(unsigned int gid) {
	return gid & TMX_FLIP_BITS_REMOVAL;	
}

void draw_image(ClientInfo &inf, tmx_map &map, tmx_layer &layer, Camera &cam) {

}

void draw_tiles(ClientInfo &inf, tmx_map &map, tmx_layer &layer, Camera &cam) {
	for (int y = cam.top / map.tile_height; y < cam.bottom / map.tile_height; y++) {
		for (int x = cam.left / map.tile_width; x < cam.right / map.tile_width; x++) {
			if (y < 0 || x < 0 || y >= map.height || x >= map.width) {
				continue;
			}

			unsigned int raw = layer.content.gids[(y*map.width) + x];
			unsigned int gid = gid_clear_flags(raw);

			if (gid == 0) {
				continue;
			}

			int flipX = raw & TMX_FLIPPED_HORIZONTALLY ? -1 : 1;
			int flipY = raw & TMX_FLIPPED_VERTICALLY ? -1 : 1;
			bool flipDiag = raw & TMX_FLIPPED_DIAGONALLY;

			tmx_tile *tile = map.tiles[gid];
			tmx_tileset *ts = tile->tileset;

			int sx = tile->ul_x;
			int sy = tile->ul_y;
			int w = ts->tile_width;
			int h = ts->tile_height;
			int dx = x*ts->tile_width;
			int dy = y*ts->tile_height;

			Img *img = (Img*)tile->tileset->image->resource_image;

			nvgSave(img->nvg);

			nvgTranslate(img->nvg, dx + (w / 2), dy + (h / 2));

			if (flipDiag) {
				nvgTransform(img->nvg, 0, 1, 1, 0, 0, 0);
			}

			if (flipX == -1 ^ flipY == -1 && flipDiag) {
				nvgScale(img->nvg, flipY, flipX);
			}
			else {
				nvgScale(img->nvg, flipX, flipY);
			}

			auto paint = nvgImagePattern(img->nvg, -(w / 2) - sx, -(h / 2) - sy, img->w, img->h, 0, img->hnd, 1.0f);
			nvgBeginPath(img->nvg);
			nvgRect(img->nvg, -(w / 2), -(h / 2), w, h);
			nvgFillPaint(img->nvg, paint);
			nvgFill(img->nvg);

			nvgRestore(img->nvg);
		}
	}
}

void TileMapDrawSystem::update(double dt)
{
	BaseWorld *world = (BaseWorld*)this->world;
	Camera *activeCam = nullptr;

	for (auto &cam : world->Cameras) {
		if (cam.active) {
			activeCam = &cam;
			break;
		}
	}

	if (activeCam == nullptr) {
		trap->Error(ERR_DROP, "can't draw tilemap without an active camera");
	}

	for (auto &entity : world->entities) {
		PECS_SKIP_INVALID_ENTITY;

		auto &tileMap = world->TileMaps[entity.id];

		nvgBeginPath(inf->nvg);
		nvgRect(inf->nvg, activeCam->left, activeCam->top, activeCam->size.x / activeCam->scale, activeCam->size.y / activeCam->scale);
		nvgFillColor(inf->nvg, nvgRGBA(
			(tileMap.map->backgroundcolor >> 16) & 0xFF,
			(tileMap.map->backgroundcolor >> 8) & 0xFF,
			(tileMap.map->backgroundcolor) & 0xFF,
			255
		));
		nvgFill(inf->nvg);

		auto layer = tileMap.map->ly_head;
		while (layer) {
			if (layer->visible == false || layer->type == L_OBJGR) {
				layer = layer->next;
				continue;
			}

			if (layer->type == L_IMAGE) {
				draw_image(*inf, *tileMap.map, *layer, *activeCam);
			}
			else if (layer->type == L_LAYER) {
				draw_tiles(*inf, *tileMap.map, *layer, *activeCam);
			}

			layer = layer->next;
		}
	}
}