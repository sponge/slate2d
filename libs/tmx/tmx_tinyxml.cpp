#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyxml2.h"

#include "tmx.h"
#include "tmx_utils.h"

using namespace tinyxml2;

char *tmx_copyString(const char *in) {
	char	*out;
	out = (char *)malloc(strlen(in) + 1);
	strcpy(out, in);
	return out;
}

static bool parse_properties(XMLElement *ele, tmx_property **prop_headadr);

static bool parse_data(XMLElement *ele, int32_t **gidsadr, size_t gidscount) {
	const char *value, *inner_xml;

	value = ele->Attribute("encoding");
	if (!value) { /* encoding */
		tmx_err(E_MISSEL, "xml parser: missing 'encoding' attribute in the 'data' element");
		return false;
	}

	inner_xml = ele->GetText();
	if (!value) {
		tmx_err(E_XDATA, "xml parser: missing content in the 'data' element");
		return false;
	}

	if (!strcmp(value, "base64")) {
		auto compression = ele->Attribute("compression");
		if (!compression) { /* compression */
			tmx_err(E_MISSEL, "xml parser: missing 'compression' attribute in the 'data' element");
			return false;
		}
		if (strcmp(compression, "zlib") && strcmp(compression, "gzip")) {
			tmx_err(E_ENCCMP, "xml parser: unsupported data compression: '%s'", value); /* unsupported compression */
			return false;
		}
		if (!data_decode(inner_xml, B64Z, gidscount, gidsadr)) {
			return false;
		};

	}
	else if (!strcmp(value, "xml")) {
		tmx_err(E_ENCCMP, "xml parser: unimplemented data encoding: XML");
		return false;
	}
	else if (!strcmp(value, "csv")) {
		if (!data_decode(inner_xml, CSV, gidscount, gidsadr)) {
			return false;
		}
	}
	else {
		tmx_err(E_ENCCMP, "xml parser: unknown data encoding: %s", value);
		return false;
	}
	return true;
}

static bool parse_points(XMLElement *ele, double ***ptsarrayadr, int *ptslenadr) {
	const char *value, *v;
	int i;

	value = ele->Attribute("points");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'points' attribute in the 'object' element");
		return false;
	}

	*ptsarrayadr = (double**)tmx_alloc_func(NULL, *ptslenadr * sizeof(double*)); /* points[i][x,y] */
	if (!(*ptsarrayadr)) {
		tmx_errno = E_ALLOC;
		return false;
	}

	(*ptsarrayadr)[0] = (double*)tmx_alloc_func(NULL, *ptslenadr * 2 * sizeof(double));
	if (!(*ptsarrayadr)[0]) {
		tmx_free_func(*ptsarrayadr);
		tmx_errno = E_ALLOC;
		return false;
	}

	for (i = 1; i<*ptslenadr; i++) {
		(*ptsarrayadr)[i] = (*ptsarrayadr)[0] + (i * 2);
	}

	v = tmx_copyString(value);
	for (i = 0; i<*ptslenadr; i++) {
		if (sscanf(v, "%lf,%lf", (*ptsarrayadr)[i], (*ptsarrayadr)[i] + 1) != 2) {
			tmx_err(E_XDATA, "xml parser: corrupted point list");
			return false;
		}
		v = 1 + strchr(v, ' ');
	}

	return true;
}

static bool parse_image(XMLElement *ele, tmx_image **img_adr, bool strict, const char *filename) {
	const char *value;

	auto res = alloc_image();
	if (!res) {
		return false;
	}

	*img_adr = res;

	value = ele->Attribute("source");
	if (value) {
		res->source = tmx_copyString(value);
		if (!(load_image(&(res->resource_image), filename, value))) {
			tmx_err(E_UNKN, "xml parser: an error occured in the delegated image loading function");
			return false;
		}
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'source' attribute in the 'image' element");
		return false;
	}

	value = ele->Attribute("height");
	if (value) {
		res->height = atoi(value);
	}
	else if (strict) {
		tmx_err(E_MISSEL, "xml parser: missing 'height' attribute in the 'image' element");
		return false;
	}

	value = ele->Attribute("width");
	if (value) {
		res->width = atoi(value);
	}
	else if (strict) {
		tmx_err(E_MISSEL, "xml parser: missing 'width' attribute in the 'image' element");
		return 0;
	}

	value = ele->Attribute("trans");
	if (value) {
		res->trans = get_color_rgb(value);
		res->uses_trans = 1;
	}

	return true;
}

static bool parse_tileoffset(XMLElement *ele, int *x, int *y) {
	const char *value;

	value = ele->Attribute("x");
	if (value) {
		*x = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'x' attribute in the 'tileoffset' element");
		return false;
	}

	value = ele->Attribute("x");
	if (value) {
		*y = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'y' attribute in the 'tileoffset' element");
		return false;
	}

	return true;
}

static bool parse_object(XMLElement *ele, tmx_object *obj) {
	const char *value;

	/* parses each attribute */
	value = ele->Attribute("id");
	if (value) {
		obj->id = atoi(value);
	}
	else {
		printf("%s", ele->GetText());
		tmx_err(E_MISSEL, "xml parser: missing 'id' attribute in the 'object' element");
		return false;
	}

	value = ele->Attribute("x");
	if (value) {
		obj->x = atof(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'x' attribute in the 'object' element");
		return false;
	}

	value = ele->Attribute("y");
	if (value) {
		obj->y = atof(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'y' attribute in the 'object' element");
		return false;
	}

	value = ele->Attribute("name");
	if (value) {
		obj->name = tmx_copyString(value);
	}

	value = ele->Attribute("type");
	if (value) {
		obj->type = tmx_copyString(value);
	}

	value = ele->Attribute("visible");
	if (value) {
		obj->visible = (char)atoi(value);
	}

	value = ele->Attribute("height");
	if (value) {
		obj->shape = S_SQUARE;
		obj->height = atof(value);
	}

	value = ele->Attribute("width");
	if (value) {
		obj->width = atof(value);
	}

	value = ele->Attribute("gid");
	if (value) {
		obj->shape = S_TILE;
		obj->gid = atoi(value);
	}

	value = ele->Attribute("rotation");
	if (value) {
		obj->rotation = atof(value);
	}

	auto subEle = ele->FirstChildElement();
	while (subEle) {
		auto name = subEle->Value();
		if (!strcmp(name, "properties")) {
			if (!parse_properties(subEle, &(obj->properties))) return 0;
		}
		else if (!strcmp(name, "ellipse")) {
			obj->shape = S_ELLIPSE;
		}
		else {
			if (!strcmp(name, "polygon")) {
				obj->shape = S_POLYGON;
			}
			else if (!strcmp(name, "polyline")) {
				obj->shape = S_POLYLINE;
			}
			else {
				return false;
			}

			if (!parse_points(subEle, &(obj->points), &(obj->points_len))) {
				return false;
			}
		}

		subEle = ele->NextSiblingElement();
	}

	return true;
}

static tmx_anim_frame* parse_animation(XMLElement *ele, int frame_count, unsigned int *length) {
	// FIXME: weird recursion thing, dunno how to port it without understanding libxml2 more
	//const char *value;
	//int curr_depth = 0;
	//tmx_anim_frame frame;
	//tmx_anim_frame *res;

	//curr_depth++;

	//value = ele->Value();
	//if (strcmp(value, "frame")) {
	//	tmx_err(E_XDATA, "xml parser: invalid element '%s' within an 'animation'", value);
	//	return false;
	//}

	//value = ele->Attribute("tileid");
	//if (value) {
	//	frame.tile_id = atoi(value);
	//}
	//else {
	//	tmx_err(E_MISSEL, "xml parser: missing 'tileid' attribute in the 'frame' element");
	//	return false;
	//}

	//value = ele->Attribute("duration");
	//if (value) {
	//	frame.duration = atoi(value);
	//}
	//else {
	//	tmx_err(E_MISSEL, "xml parser: missing 'duration' attribute in the 'frame' element");
	//	return false;
	//}

	return nullptr;
}

static bool parse_tile(XMLElement *ele, tmx_tileset *tileset, const char *filename) {
	tmx_tile *res = NULL;
	const char *value;

	value = ele->Attribute("id");
	if (value) {
		int to_move;
		unsigned int id = atoi(value);
		/* Insertion sort */
		auto len = tileset->user_data.integer;
		for (to_move = 0; (len - 1) - to_move >= 0; to_move++) {
			if (tileset->tiles[(len - 1) - to_move].id < id) {
				break;
			}
		}
		if (to_move > 0) {
			memmove((tileset->tiles) + (len - to_move + 1), (tileset->tiles) + (len - to_move), to_move * sizeof(tmx_tile));
		}
		res = &(tileset->tiles[len - to_move]);

		if ((unsigned int)(tileset->user_data.integer) == tileset->tilecount) {
			tileset->user_data.integer = 0;
		}
		else {
			tileset->user_data.integer += 1;
		}
		/* --- */
		res->id = id;
		res->tileset = tileset;
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'id' attribute in the 'tile' element");
		return false;
	}

	auto subEle = ele->FirstChildElement();
	while (subEle) {
		bool success = false;

		auto name = subEle->Value();
		if (!strcmp(name, "properties")) {
			success = parse_properties(subEle, &(res->properties));
		}
		else if (!strcmp(name, "image")) {
			success = parse_image(subEle, &(res->image), 0, filename);
		}
		else if (!strcmp(name, "objectgroup")) {
			auto objEle = subEle->FirstChildElement("object");
			while (objEle) {
				auto objRes = alloc_object();
				if (!objRes) {
					return false;
				}

				objRes->next = res->collision;
				res->collision = objRes;

				success = parse_object(objEle, objRes);

				if (!success) {
					return false;
				}

				objEle = objEle->NextSiblingElement("object");
			}

		}
		else if (!strcmp(name, "animation")) {
			auto frameEle = subEle->FirstChildElement("frame");
			while (frameEle) {
				res->animation = parse_animation(frameEle, 0, &(res->animation_len));
				if (!res->animation) {
					return false;
				}
				frameEle = frameEle->NextSiblingElement("frame");
			}
			success = true;
		}

		if (!success) {
			return false;
		}

		subEle = subEle->NextSiblingElement();
	}

	return true;
}

static bool parse_tileset_sub(XMLElement *ele, tmx_tileset *ts_addr, const char *filename) {
	const char *value;

	value = ele->Attribute("name");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'name' attribute in the 'tileset' element");
		return false;
	}
	else {
		ts_addr->name = tmx_copyString(value);

	}

	value = ele->Attribute("tilecount");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tilecount' attribute in the 'tileset' element");
		return false;
	}
	else {
		ts_addr->tilecount = atoi(value);
	}

	value = ele->Attribute("tilewidth");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tilewidth' attribute in the 'tileset' element");
		return false;
	}
	else {
		ts_addr->tile_width = atoi(value);
	}

	value = ele->Attribute("tileheight");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tileheight' attribute in the 'tileset' element");
		return false;
	}
	else {
		ts_addr->tile_height = atoi(value);
	}

	value = ele->Attribute("spacing");
	if (value) {
		ts_addr->spacing = atoi(value);
	}

	value = ele->Attribute("margin");
	if (value) {
		ts_addr->margin = atoi(value);
	}

	ts_addr->tiles = alloc_tiles(ts_addr->tilecount);
	if (!ts_addr->tiles) {
		return false;
	}

	ts_addr->tiles = alloc_tiles(ts_addr->tilecount);
	if (!ts_addr->tiles) {
		return false;
	}

	auto subEle = ele->FirstChildElement();
	while (subEle) {
		auto name = subEle->Value();
		bool success = false;

		if (!strcmp(name, "image")) {
			success = parse_image(subEle, &(ts_addr->image), true, filename);
		}
		else if (!strcmp(name, "tileoffset")) {
			success = parse_tileoffset(subEle, &(ts_addr->x_offset), &(ts_addr->y_offset));
		}
		else if (!strcmp(name, "properties")) {
			success = parse_properties(subEle, &(ts_addr->properties));
		}
		else if (!strcmp(name, "tile")) {
			success = parse_tile(subEle, ts_addr, filename);
		}

		if (!success) {
			return false;
		}

		subEle = subEle->NextSiblingElement();
	}

	if (ts_addr->image && !set_tiles_runtime_props(ts_addr)) {
		return false;
	}

	return true;
}

static bool parse_tileset(XMLElement *ele, tmx_tileset **ts_headadr, const char *filename) {
	auto res = alloc_tileset();
	if (!res) {
		return false;
	}
	res->next = *ts_headadr;
	*ts_headadr = res;

	auto firstgid = ele->Attribute("firstgid");
	if (!firstgid) {
		tmx_err(E_MISSEL, "xml parser: missing 'firstgid' attribute in the 'tileset' element");
		return false;
	}

	res->firstgid = atoi(firstgid);

	auto source = ele->Attribute("source");
	if (source) {
		XMLDocument *doc = new XMLDocument();
		auto ab_path = mk_absolute_path(filename, source);
		XMLError err;

		if (tmx_file_read_func) {
			int sz = 0;
			const char *str = (const char *)tmx_file_read_func(ab_path, &sz);
			err = doc->Parse(str, sz);
			tmx_free_func((void*)str);
		}
		else {
			err = doc->LoadFile(ab_path);
		}

		if (err != XML_SUCCESS) {
			return false;
		}

		auto tsNode = doc->FirstChildElement();
		bool success = parse_tileset_sub(tsNode, res, ab_path);
		delete doc;
		return success;
	}
	else {
		bool success = parse_tileset_sub(ele, res, filename);
		return success;
	}
}

static bool parse_layer(XMLElement *ele, tmx_layer **layer_headadr, int map_h, int map_w, enum tmx_layer_type type, const char *filename) {
	
	auto res = alloc_layer();
	if (!res) {
		return false;
	}

	res->type = type;

	while (*layer_headadr) {
		layer_headadr = &((*layer_headadr)->next);
	}
	*layer_headadr = res;

	const char *value;
	value = ele->Attribute("name");
	if (value) {
		res->name = tmx_copyString(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'name' attribute in the 'layer' element");
		return false;
	}

	value = ele->Attribute("visible");
	if (value) {
		res->visible = atoi(value) == 1;
	}

	value = ele->Attribute("opacity");
	if (value) {
		res->opacity = strtod(value, nullptr);
	}

	value = ele->Attribute("offsetx");
	if (value) {
		res->offsetx = atoi(value);
	}

	value = ele->Attribute("offsety");
	if (value) {
		res->offsety = atoi(value);
	}

	if (type == L_OBJGR) {
		auto *objgr = alloc_objgr();
		if (!objgr) {
			return false;
		}

		res->content.objgr = objgr;

		value = ele->Attribute("color");
		if (value) { /* color */
			objgr->color = get_color_rgb(value);
		}

		value = ele->Attribute("draworder");
		objgr->draworder = parse_objgr_draworder(value);
	}

	auto layerChild = ele->FirstChildElement();
	while (layerChild) {
		auto name = layerChild->Value();
		bool success = false;

		if (!strcmp(name, "data")) {
			success = parse_data(layerChild, &(res->content.gids), map_h * map_w);
		}
		else if (!strcmp(name, "image")) {
			success = parse_image(layerChild, &(res->content.image), 0, filename);
		}
		else if (!strcmp(name, "object")) {
			auto obj = alloc_object();
			if (!obj) {
				return false;
			}

			obj->next = res->content.objgr->head;
			res->content.objgr->head = obj;

			if (!parse_object(layerChild, obj)) {
				return false;
			}

			success = true;
		}
		else if (!strcmp(name, "properties")) {
			success = parse_properties(layerChild, &(res->properties));
		}

		if (!success) {
			return false;
		}

		layerChild = layerChild->NextSiblingElement();
	}
	return true;
}

static bool parse_properties(XMLElement *ele, tmx_property **prop_headadr) {
	auto prop = ele->FirstChildElement("property");
	while (prop) {
		auto res = alloc_prop();
		if (!res) {
			return false;
		}

		res->name = tmx_copyString(prop->Attribute("name"));
		res->value = tmx_copyString(prop->Attribute("value"));

		if (!res->name || !res->value) {
			tmx_err(E_MISSEL, "xml parser: missing 'name' or 'value' attribute in the 'property' element");
			return false;
		}

		res->next = *prop_headadr;
		*prop_headadr = res;

		prop = prop->NextSiblingElement("property");
	}
	return true;
}

tmx_map *parse_root_map(XMLDocument *doc, const char *filename) {
	tmx_map *res = NULL;

	auto mapNode = doc->FirstChildElement();
	auto name = mapNode->ToElement()->Value();;

	if (strcmp(name, "map")) {
		tmx_err(E_XDATA, "xml parser: root is not a 'map' element");
		return NULL;
	}

	if (!(res = alloc_map())) return NULL;

	const char *value;
	XMLElement *mapChild;

	value = mapNode->Attribute("orientation");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'orientation' attribute in the 'map' element");
		goto cleanup;
	}

	res->orient = parse_orient(value);
	if (res->orient == O_NONE) {
		tmx_err(E_XDATA, "xml parser: unsupported 'orientation' '%s'", value);
		goto cleanup;
	}

	value = mapNode->Attribute("staggerindex");
	if (value != NULL && (res->stagger_index = parse_stagger_index(value), res->stagger_index == SI_NONE)) {
		tmx_err(E_XDATA, "xml parser: unsupported 'staggerindex' '%s'", value);
		goto cleanup;
	}

	value = mapNode->Attribute("staggeraxis");
	if (res->stagger_axis = parse_stagger_axis(value), res->stagger_axis == SA_NONE) {
		tmx_err(E_XDATA, "xml parser: unsupported 'staggeraxis' '%s'", value);
		goto cleanup;
	}

	value = mapNode->Attribute("renderorder");
	if (res->renderorder = parse_renderorder(value), res->renderorder == R_NONE) {
		tmx_err(E_XDATA, "xml parser: unsupported 'renderorder' '%s'", value);
		goto cleanup;
	}

	value = mapNode->Attribute("height");
	if (value) {
		res->height = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'height' attribute in the 'map' element");
		goto cleanup;
	}

	value = mapNode->Attribute("width");
	if (value) {
		res->width = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'width' attribute in the 'map' element");
		goto cleanup;
	}

	value = mapNode->Attribute("tileheight");
	if (value) {
		res->tile_height = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'tileheight' attribute in the 'map' element");
		goto cleanup;
	}

	value = mapNode->Attribute("tilewidth");
	if (value) {
		res->tile_width = atoi(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'tilewidth' attribute in the 'map' element");
		goto cleanup;
	}

	if ((value = mapNode->Attribute("backgroundcolor"))) {
		res->backgroundcolor = get_color_rgb(value);
	}

	if ((value = mapNode->Attribute("hexsidelength"))) {
		res->hexsidelength = atoi(value);
	}

	mapChild = mapNode->FirstChildElement();
	while (mapChild) {
		auto name = mapChild->Value();
		bool success = false;

		if (!strcmp(name, "tileset")) {
			success = parse_tileset(mapChild, &(res->ts_head), filename);
		}
		else if (!strcmp(name, "layer")) {
			success = parse_layer(mapChild, &(res->ly_head), res->height, res->width, L_LAYER, filename);
		}
		else if (!strcmp(name, "objectgroup")) {
			success = parse_layer(mapChild, &(res->ly_head), res->height, res->width, L_OBJGR, filename);
		}
		else if (!strcmp(name, "imagelayer")) {
			success = parse_layer(mapChild, &(res->ly_head), res->height, res->width, L_IMAGE, filename);
			success = true;
		}
		else if (!strcmp(name, "properties")) {
			success = parse_properties(mapChild, &(res->properties));
		}

		if (!success) {
			return nullptr;
		}

		mapChild = mapChild->NextSiblingElement();
	}

	return res;

cleanup:
	tmx_map_free(res);
	return nullptr;
}


tmx_map *parse_tinyxml(const char *filename) {
	XMLDocument *doc = new XMLDocument();

	XMLError err;

	if (tmx_file_read_func) {
		int sz = 0;
		const char *str = (const char *) tmx_file_read_func(filename, &sz);
		err = doc->Parse(str, sz);
		if (err == XML_SUCCESS) {
			tmx_free_func((void*)str);
		}
	}
	else {
		err = doc->LoadFile(filename);
	}

	if (err != XML_SUCCESS) {
		return nullptr;
	}

	auto res = parse_root_map(doc, filename);
	delete doc;

	return res;
}