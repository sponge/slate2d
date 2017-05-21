#include "systems.h"
#include "../baseworld.h"
#include "../public.h"
#include "../drawcommands.h"

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

			byte flipBits = (raw & TMX_FLIPPED_HORIZONTALLY ? FLIP_H : 0) | (raw & TMX_FLIPPED_VERTICALLY ? FLIP_V : 0) | (raw & TMX_FLIPPED_DIAGONALLY ? FLIP_DIAG : 0);

			tmx_tile *tile = map.tiles[gid];
			tmx_tileset *ts = tile->tileset;

			Img *img = (Img*)tile->tileset->image->resource_image;

			DC_DrawImage(
				x*ts->tile_width + (ts->tile_width / 2), y*ts->tile_height + (ts->tile_height / 2),
				ts->tile_width, ts->tile_height,
				tile->ul_x, tile->ul_y,
				1.0, flipBits, img->index
			);
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

		DC_SetColor((tileMap.map->backgroundcolor >> 16) & 0xFF,
			(tileMap.map->backgroundcolor >> 8) & 0xFF,
			(tileMap.map->backgroundcolor) & 0xFF,
			255
		);
		DC_DrawRect(activeCam->left, activeCam->top, activeCam->size.x / activeCam->scale, activeCam->size.y / activeCam->scale);

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