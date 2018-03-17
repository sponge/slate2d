#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tinyxml2.h"
extern "C" {
#include "tmx.h"
#include "tsx.h"
#include "tmx_utils.h"
}


using namespace tinyxml2;

char *tmx_copyString(const char *in) {
	char	*out;
	out = (char *)malloc(strlen(in) + 1);
	strcpy(out, in);
	return out;
}

static int parse_property(XMLElement *ele, tmx_property *prop) {
	const char *name = ele->Attribute("name");
	const char *value = ele->Attribute("value");
	const char *type = ele->Attribute("type");

	if (name == nullptr) {
		tmx_err(E_MISSEL, "xml parser: missing 'name' attribute in the 'property' element");
		assert(false); return 0;
	}

	prop->name = tmx_copyString(name);

	if (type != nullptr) { /* type */
		prop->type = parse_property_type(value);
	}
	else {
		prop->type = PT_STRING;
	}

	if (value != nullptr) {
		switch (prop->type) {
		case PT_INT:
			prop->value.integer = atoi(value);
			break;
		case PT_FLOAT:
			prop->value.decimal = atof(value);
			break;
		case PT_BOOL:
			prop->value.integer = parse_boolean(value);
			break;
		case PT_COLOR:
			prop->value.integer = get_color_rgb(value);
			break;
		case PT_NONE:
		case PT_STRING:
		case PT_FILE:
		default:
			prop->value.string = tmx_copyString(value);
			break;
		}
	}
	else if (prop->type == PT_NONE || prop->type == PT_STRING) {
		const char *inner = ele->GetText();
		prop->value.string = tmx_copyString(inner);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'value' attribute in the 'property' element");
		assert(false); return 0;
	}

	return 1;
}

static int parse_properties(XMLElement *ele, tmx_properties **prop_hashptr) {
	tmx_property *res;

	if (*prop_hashptr == NULL) {
		if (!(*prop_hashptr = (tmx_properties*)mk_hashtable(5))) {
			assert(false); return 0;
		}
	}

	auto prop = ele->FirstChildElement("property");
	while (prop) {
		if (!(res = alloc_prop())) {
			assert(false); return 0;
		}
		if (!parse_property(prop, res)) {
			assert(false); return 0;
		}
		hashtable_set((void*)*prop_hashptr, res->name, (void*)res, NULL);

		prop = prop->NextSiblingElement("property");
	}

	return 1;
}

static int parse_points(XMLElement *ele, tmx_shape *shape) {
	const char *value, *v;
	int i;

	value = ele->Attribute("points");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'points' attribute in the 'object' element");
		assert(false); return 0;
	}

	shape->points_len = 1 + count_char_occurences(value, ' ');

	shape->points = (double**)tmx_alloc_func(NULL, shape->points_len * sizeof(double*)); /* points[i][x,y] */
	if (!(shape->points)) {
		tmx_errno = E_ALLOC;
		assert(false); return 0;
	}

	shape->points[0] = (double*)tmx_alloc_func(NULL, shape->points_len * 2 * sizeof(double));
	if (!(shape->points[0])) {
		tmx_free_func(shape->points);
		tmx_errno = E_ALLOC;
		assert(false); return 0;
	}

	for (i = 1; i<shape->points_len; i++) {
		shape->points[i] = shape->points[0] + (i * 2);
	}

	v = value;
	for (i = 0; i<shape->points_len; i++) {
		if (sscanf(v, "%lf,%lf", shape->points[i], shape->points[i] + 1) != 2) {
			tmx_err(E_XDATA, "xml parser: corrupted point list");
			assert(false); return 0;
		}
		v = 1 + strchr(v, ' ');
	}

	return 1;
}

static int parse_text(XMLElement *ele, tmx_text *text) {
	const char *value;

	value = ele->Attribute("fontfamily");
	if (value != nullptr) {
		text->fontfamily = tmx_copyString(value);
	}
	else {
		text->fontfamily = tmx_strdup("sans-serif");
	}

	value = ele->Attribute("pixelsize");
	if (value != nullptr) {
		text->pixelsize = (int)atoi(value);
	}

	value = ele->Attribute("color");
	if (value != nullptr) {
		text->color = get_color_rgb(value);
	}

	value = ele->Attribute("wrap");
	if (value != nullptr) {
		text->color = (int)atoi(value);
	}

	value = ele->Attribute("bold");
	if (value != nullptr) {
		text->bold = (int)atoi(value);
	}

	value = ele->Attribute("italic");
	if (value != nullptr) {
		text->italic = (int)atoi(value);
	}

	value = ele->Attribute("underline");
	if (value != nullptr) {
		text->underline = (int)atoi(value);
	}

	value = ele->Attribute("strokeout");
	if (value != nullptr) {
		text->strikeout = (int)atoi(value);
	}

	value = ele->Attribute("kerning");
	if (value != nullptr) {
		text->kerning = (int)atoi(value);
	}

	value = ele->Attribute("halign");
	if (value != nullptr) {
		text->halign = parse_horizontal_align(value);
	}

	value = ele->Attribute("valign");
	if (value != nullptr) {
		text->valign = parse_vertical_align(value);
	}

	value = ele->GetText();
	if (value != nullptr) {
		text->text = tmx_copyString(value);
	}

	return 1;
}

static int parse_object(XMLElement *ele, tmx_object *obj) {
	const char *value;

	/* parses each attribute */
	value = ele->Attribute("id");
	if (value) {
		obj->id = atoi(value);
	}
	else {
		printf("%s", ele->GetText());
		tmx_err(E_MISSEL, "xml parser: missing 'id' attribute in the 'object' element");
		assert(false); return 0;
	}

	value = ele->Attribute("x");
	if (value) {
		obj->x = atof(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'x' attribute in the 'object' element");
		assert(false); return 0;
	}

	value = ele->Attribute("y");
	if (value) {
		obj->y = atof(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'y' attribute in the 'object' element");
		assert(false); return 0;
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
		obj->obj_type = OT_SQUARE;
		obj->height = atof(value);
	}

	value = ele->Attribute("width");
	if (value) {
		obj->width = atof(value);
	}

	value = ele->Attribute("gid");
	if (value) {
		obj->obj_type = OT_TILE;
		obj->content.gid = atoi(value);
	}

	value = ele->Attribute("rotation");
	if (value) {
		obj->rotation = atof(value);
	}

	auto subEle = ele->FirstChildElement();
	while (subEle) {
		auto name = subEle->Value();
		if (!strcmp(name, "properties")) {
			if (!parse_properties(subEle, &(obj->properties))) {
				assert(false);  return 0;
			}
		}
		else if (!strcmp(name, "ellipse")) {
			obj->obj_type = OT_ELLIPSE;
		}
		else {
			if (!strcmp(name, "polygon")) {
				obj->obj_type = OT_POLYGON;
			}
			else if (!strcmp(name, "polyline")) {
				obj->obj_type = OT_POLYLINE;
			}
			else if (!strcmp(name, "text")) {
				obj->obj_type = OT_TEXT;
			}
			else {
				assert(false); return 0;
			}

			if (obj->obj_type == OT_POLYGON || obj->obj_type == OT_POLYLINE) {
				if (obj->content.shape = alloc_shape(), !(obj->content.shape)) {
					assert(false); return 0;
				}
				if (!parse_points(subEle, obj->content.shape)) {
					assert(false); return 0;
				}
			}
			else if (obj->obj_type == OT_TEXT) {
				if (obj->content.text = alloc_text(), !(obj->content.text)) {
					assert(false); return 0;
				}
				if (!parse_text(subEle, obj->content.text)) {
					assert(false); return 0;
				}
			}
		}

		subEle = ele->NextSiblingElement();
	}

	return 1;
}

static int parse_data(XMLElement *ele, int32_t **gidsadr, size_t gidscount) {
	const char *value, *inner_xml;

	value = ele->Attribute("encoding");
	if (!value) { /* encoding */
		tmx_err(E_MISSEL, "xml parser: missing 'encoding' attribute in the 'data' element");
		assert(false); return 0;
	}

	inner_xml = ele->GetText();
	if (!value) {
		tmx_err(E_XDATA, "xml parser: missing content in the 'data' element");
		assert(false); return 0;
	}

	if (!strcmp(value, "base64")) {
		auto compression = ele->Attribute("compression");
		if (!compression) { /* compression */
			tmx_err(E_MISSEL, "xml parser: missing 'compression' attribute in the 'data' element");
			assert(false); return 0;
		}
		if (strcmp(compression, "zlib") && strcmp(compression, "gzip")) {
			tmx_err(E_ENCCMP, "xml parser: unsupported data compression: '%s'", value); /* unsupported compression */
			assert(false); return 0;
		}
		if (!data_decode(inner_xml, B64Z, gidscount, gidsadr)) {
			assert(false); return 0;
		};

	}
	else if (!strcmp(value, "xml")) {
		tmx_err(E_ENCCMP, "xml parser: unimplemented data encoding: XML");
		assert(false); return 0;
	}
	else if (!strcmp(value, "csv")) {
		if (!data_decode(inner_xml, CSV, gidscount, gidsadr)) {
			assert(false); return 0;
		}
	}
	else {
		tmx_err(E_ENCCMP, "xml parser: unknown data encoding: %s", value);
		assert(false); return 0;
	}
	return 1;
}

static int parse_image(XMLElement *ele, tmx_image **img_adr, short strict, const char *filename) {
	const char *value;

	auto res = alloc_image();
	if (!res) {
		assert(false); return 0;
	}

	*img_adr = res;

	value = ele->Attribute("source");
	if (value) {
		res->source = tmx_copyString(value);
		if (!(load_image(&(res->resource_image), filename, value))) {
			tmx_err(E_UNKN, "xml parser: an error occured in the delegated image loading function");
			assert(false); return 0;
		}
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'source' attribute in the 'image' element");
		assert(false); return 0;
	}

	value = ele->Attribute("height");
	if (value) {
		res->height = atoi(value);
	}
	else if (strict) {
		tmx_err(E_MISSEL, "xml parser: missing 'height' attribute in the 'image' element");
		assert(false); return 0;
	}

	value = ele->Attribute("width");
	if (value) {
		res->width = atoi(value);
	}
	else if (strict) {
		tmx_err(E_MISSEL, "xml parser: missing 'width' attribute in the 'image' element");
		assert(false); return 0;
	}

	value = ele->Attribute("trans");
	if (value) {
		res->trans = get_color_rgb(value);
		res->uses_trans = 1;
	}

	return 1;
}

static int parse_layer(XMLElement *ele, tmx_layer **layer_headadr, int map_h, int map_w, enum tmx_layer_type type, const char *filename) {
	auto res = alloc_layer();
	if (!res) {
		assert(false); return 0;
	}

	res->type = type;

	while (*layer_headadr) {
		layer_headadr = &((*layer_headadr)->next);
	}
	*layer_headadr = res;

	const char *value = ele->Attribute("name");
	if (value) {
		res->name = tmx_copyString(value);
	}
	else {
		tmx_err(E_MISSEL, "xml parser: missing 'name' attribute in the 'layer' element");
		assert(false); return 0;
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
			assert(false); return 0;
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
		enum tmx_layer_type child_type = parse_layer_type(name);

		if (!strcmp(name, "data")) {
			success = parse_data(layerChild, &(res->content.gids), map_h * map_w);
		}
		else if (!strcmp(name, "image")) {
			success = parse_image(layerChild, &(res->content.image), 0, filename);
		}
		else if (!strcmp(name, "object")) {
			auto obj = alloc_object();
			if (!obj) {
				assert(false); return 0;
			}

			obj->next = res->content.objgr->head;
			res->content.objgr->head = obj;

			if (!parse_object(layerChild, obj)) {
				assert(false); return 0;
			}

			success = true;
		}
		else if (!strcmp(name, "properties")) {
			success = parse_properties(layerChild, &(res->properties));
		}
		else if (type == L_GROUP && child_type != L_NONE) {
			if (!parse_layer(ele, &(res->content.group_head), map_h, map_w, child_type, filename)) {
				assert(false); return 0;
			}
		}

		if (!success) {
			assert(false); return 0;
		}

		layerChild = layerChild->NextSiblingElement();
	}


	return 1;
}

static int parse_tileoffset(XMLElement *ele, int *x, int *y) {
	assert(false);
	assert(false); return 0;
}

/* recursive function that alloc tmx_anim_frames on the stack and then move them to the heap */
static tmx_anim_frame* parse_animation(XMLElement *ele, int frame_count, unsigned int *length) {
	assert(false);
	assert(false); return 0;
}

static int parse_tile(XMLElement *ele, tmx_tileset *tileset, const char *filename) {
	tmx_tile *res = NULL;
	const char *value;

	value = ele->Attribute("id");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'id' attribute in the 'tile' element");
		assert(false); return 0;
	}

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

	value = ele->Attribute("type");
	if (value) {
		res->type = tmx_copyString(value);
	}

	auto subEle = ele->FirstChildElement();
	while (subEle) {
		bool success = 0;

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
					assert(false); return 0;
				}

				objRes->next = res->collision;
				res->collision = objRes;

				success = parse_object(objEle, objRes);

				if (!success) {
					assert(false); return 0;
				}

				objEle = objEle->NextSiblingElement("object");
			}

		}
		else if (!strcmp(name, "animation")) {
			auto frameEle = subEle->FirstChildElement("frame");
			while (frameEle) {
				res->animation = parse_animation(frameEle, 0, &(res->animation_len));
				if (!res->animation) {
					assert(false); return 0;
				}
				frameEle = frameEle->NextSiblingElement("frame");
			}
			success = 1;
		}

		if (!success) {
			assert(false); return 0;
		}

		subEle = subEle->NextSiblingElement();
	}

	return true;
}

/* parses a tileset within the tmx file or in a dedicated tsx file */
static int parse_tileset(XMLElement *ele, tmx_tileset *ts_addr, const char *filename) {
	const char *value;

	value = ele->Attribute("name");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'name' attribute in the 'tileset' element");
		assert(false); return 0;
	}
	else {
		ts_addr->name = tmx_copyString(value);
	}

	value = ele->Attribute("tilecount");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tilecount' attribute in the 'tileset' element");
		assert(false); return 0;
	}
	else {
		ts_addr->tilecount = atoi(value);
	}

	value = ele->Attribute("tilewidth");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tilewidth' attribute in the 'tileset' element");
		assert(false); return 0;
	}
	else {
		ts_addr->tile_width = atoi(value);
	}

	value = ele->Attribute("tileheight");
	if (!value) {
		tmx_err(E_MISSEL, "xml parser: missing 'tileheight' attribute in the 'tileset' element");
		assert(false); return 0;
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
		assert(false); return 0;
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
			assert(false); return 0;
		}

		subEle = subEle->NextSiblingElement();
	}

	if (ts_addr->image && !set_tiles_runtime_props(ts_addr)) {
		assert(false); return 0;
	}

	return 1;
}

static int parse_tileset_list(XMLElement *ele, tmx_tileset_list **ts_headadr, tmx_tileset_manager *ts_mgr, const char *filename) {
	tmx_tileset_list *res_list = NULL;
	tmx_tileset *res = NULL;
	const char *value, *ab_path;
	XMLDocument *sub_reader = new XMLDocument();

	if (!(res_list = alloc_tileset_list())) {
		assert(false); return 0;
	}
	res_list->next = *ts_headadr;
	*ts_headadr = res_list;

	auto firstgid = ele->Attribute("firstgid");
	if (!firstgid) {
		tmx_err(E_MISSEL, "xml parser: missing 'firstgid' attribute in the 'tileset' element");
		assert(false); return 0;
	}

	res_list->firstgid = atoi(firstgid);

	auto source = ele->Attribute("source");
	if (source) {
		if (ts_mgr) {
			res = (tmx_tileset*)hashtable_get((void*)ts_mgr, value);
			if (res) {
				res_list->tileset = res;
				return 1;
			}
		}

		if (!(res = alloc_tileset())) {
			assert(false); return 0;
		}
		res_list->tileset = res;
		if (ts_mgr) {
			hashtable_set((void*)ts_mgr, value, (void*)res, tileset_deallocator);
		}
		else {
			res->is_embedded = 1;
		}

		ab_path = mk_absolute_path(filename, source);
		if (ab_path == nullptr) {
			assert(false); return 0;
		}

		XMLError err;
		if (tmx_file_read_func) {
			int sz = 0;
			const char *str = (const char *)tmx_file_read_func(ab_path, &sz);
			err = sub_reader->Parse(str, sz);
			tmx_free_func((void*)str);
		}
		else {
			err = sub_reader->LoadFile(filename);
		}

		if (err != XML_SUCCESS) { /* opens */
			tmx_err(E_XDATA, "xml parser: cannot open extern tileset '%s'", ab_path);
			assert(false); return 0;
		}
		int ret = parse_tileset(sub_reader->FirstChildElement(), res, ab_path); /* and parses the tsx file */
		
		delete sub_reader;
		return ret;
	}

	/* Embedded tileset */
	if (!(res = alloc_tileset())) {
		assert(false); return 0;
	}
	res->is_embedded = 1;
	res_list->tileset = res;

	return parse_tileset(ele, res, filename);
}

static tmx_map *parse_root_map(XMLDocument *doc, tmx_tileset_manager *ts_mgr, const char *filename) {
	tmx_map *res = nullptr;

	auto mapNode = doc->FirstChildElement();
	auto name = mapNode->ToElement()->Value();;

	// FIXME: check if dtd before root?

	if (strcmp(name, "map")) {
		tmx_err(E_XDATA, "xml parser: root is not a 'map' element");
		assert(false); return nullptr;
	}

	if (!(res = alloc_map())) {
		assert(false); return nullptr;
	}

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
			success = parse_tileset_list(mapChild, &(res->ts_head), ts_mgr, filename);
		}
		else if (!strcmp(name, "properties")) {
			success = parse_properties(mapChild, &(res->properties));
		}
		else {
			tmx_layer_type layerType = parse_layer_type(name);
			success = parse_layer(mapChild, &(res->ly_head), res->height, res->width, layerType, filename);
		}

		if (!success) {
			assert(false); return nullptr;
		}

		mapChild = mapChild->NextSiblingElement();
	}

	return res;

cleanup:
	tmx_map_free(res);
	assert(false); return nullptr;
}

static tmx_tileset* parse_root_tileset(XMLElement *ele, const char *filename) {
	tmx_tileset *res;

	if (!(res = alloc_tileset())) {
		assert(false); return nullptr;
	}

	parse_tileset(ele, res, filename);

	return res;
}

/*
	Public TMX load functions
*/

extern "C" tmx_map *parse_xml(tmx_tileset_manager *ts_mgr, const char *filename) {
	assert(false); return 0;
}

extern "C" tmx_map* parse_xml_buffer(tmx_tileset_manager *ts_mgr, const char *buffer, int len) {
	XMLDocument *doc = new XMLDocument();
	XMLError err = doc->Parse(buffer, len);
	if (err != XML_SUCCESS) {
		tmx_err(E_UNKN, "xml parser: unable to create parser for buffer");
		assert(false); return nullptr;
	}

	tmx_map *res = parse_root_map(doc, ts_mgr, NULL);
	return res;
}

extern "C" tmx_map* parse_xml_fd(tmx_tileset_manager *ts_mgr, int fd) {
	assert(false); return 0;
}

extern "C" tmx_map* parse_xml_callback(tmx_tileset_manager *ts_mgr, tmx_read_functor callback, void *userdata) {
	assert(false); return 0;
}

/*
	Public TSX load functions
*/

extern "C" tmx_tileset* parse_tsx_xml(const char *filename) {
	assert(false); return 0;
}

extern "C" tmx_tileset* parse_tsx_xml_buffer(const char *buffer, int len) {
	assert(false); return 0;
}

extern "C" tmx_tileset* parse_tsx_xml_fd(int fd) {
	assert(false); return 0;
}

extern "C" tmx_tileset* parse_tsx_xml_callback(tmx_read_functor callback, void *userdata) {
	assert(false); return 0;
}
