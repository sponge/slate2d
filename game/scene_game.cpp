#include "scene_game.h"
#include <tmx.h>
#include "draw.h"
#include "public.h"
#include "map.h"

void GameScene::Startup(ClientInfo* info) {
	inf = info;
	dog = trap->Asset_Create(ASSET_IMAGE, "dog", "gfx/dog.png", 0);
	music = trap->Asset_Create(ASSET_MOD, "music", "music/frantic_-_dog_doesnt_care.it", 0);
	speech = trap->Asset_Create(ASSET_SPEECH, "speech", "great job! you are a good dog!", 0);
	font = trap->Asset_Create(ASSET_BITMAPFONT, "font", "gfx/good_neighbors.png", 0);
	trap->Asset_BMPFNT_Set(font, "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", 0, -1, 7, 16);

	map = trap->Map_Load(mapFileName);

	spr = trap->Asset_Create(ASSET_SPRITE, "spr", "gfx/sprites.gif", 0);
	trap->Asset_Sprite_Set(spr, 8, 8, 0, 0);

	trap->Asset_LoadAll();

	//trap->Snd_Play(music, 1.0f, 0.0f, true);
	//trap->Snd_Play(speech, 1.0f, 0.0f, false);

	int objLayer = Map_GetLayerByName(map, "objects");
	if (objLayer != -1) {
		tmx_object *obj = Map_LayerObjects(map, objLayer, nullptr);
		while (obj != nullptr) {
			obj = Map_LayerObjects(map, objLayer, obj);
		}
	}
}

void GameScene::Update(double dt) {

}

void GameScene::Render() {
	DC_Clear();
	DC_ResetTransform();
	DC_Transform((float)(inf->width / 320), 0, 0, (float)(inf->width / 320), 0, 0);

	DC_DrawMapLayer(0, 0.0f, 0.0f - map->height * map->tile_height + 180);

	DC_SetColor(COLOR_FILL, 255, 0, 0, 255);
	DC_DrawRect(5, 3, 16, 16);

	DC_DrawImage(dog, 120, 120, 154, 16);
	DC_DrawBmpText(font, 32, 50, "Good Dog!");

	DC_DrawSprite(spr, 265, 300, 150, 1.0f, 1.0f, 0, 3, 3);
	DC_DrawSprite(spr, 265, 280, 150, 0.25f, 1.0f, 0, 3, 3);

	DC_SetColor(COLOR_FILL, 60, 0, 90, 255);
	DC_SetScissor(0, 0, 280, 110);
	DC_DrawCircle(270, 100, 20);
	DC_ResetScissor();

	DC_SetColor(COLOR_STROKE, 0, 255, 0, 255);
	DC_DrawLine(0, 0, 320, 180);

	DC_SetColor(COLOR_STROKE, 130, 140, 150, 255);
	DC_DrawRect(10, 30, 64, 64, OUTLINE);

	DC_SetColor(COLOR_STROKE, 255, 255, 0, 255);
	DC_DrawCircle(200, 25, 16, OUTLINE);

	DC_SetColor(COLOR_FILL, 140, 90, 40, 255);
	DC_DrawCircle(200, 70, 16);

	DC_SetColor(COLOR_FILL, 40, 90, 40, 255);
	DC_DrawTri(150, 150, 160, 160, 130, 160);

	DC_SetColor(COLOR_STROKE, 0, 255, 255, 255);
	DC_DrawTri(150, 180, 170, 170, 180, 180, OUTLINE);

	DC_Submit();
}

GameScene::~GameScene()
{
	trap->Map_Free(map);
	trap->Asset_ClearAll();
}
