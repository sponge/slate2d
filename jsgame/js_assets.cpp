#include "../src/slate2d.h"
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

static JSValue js_assets_load(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (!JS_IsObject(argv[0])) return JS_ThrowTypeError(ctx, "options arg is not an object");

  JSValueConst assetType = JS_GetPropertyStr(ctx, argv[0], "type");
  const char *assetTypeStr = JS_ToCString(ctx, assetType);
  JS_FreeValue(ctx, assetType);

  if (assetTypeStr == nullptr) {
    return JS_ThrowTypeError(ctx, "options.type is missing or not a string");
  }

  AssetHandle hnd;

  JSValueConst name = JS_GetPropertyStr(ctx, argv[0], "name");
  const char *nameStr = JS_ToCString(ctx, name);
  JS_FreeValue(ctx, name);

  if (assetTypeStr == nullptr) {
    return JS_ThrowTypeError(ctx, "options.name is missing or not a string");
  }

  if (strcmp(assetTypeStr, "image") == 0) {
    JSValueConst path = JS_GetPropertyStr(ctx, argv[0], "path");
    const char *pathStr = JS_ToCString(ctx, path);

    JSValueConst linearFilterVal = JS_GetPropertyStr(ctx, argv[0], "linearFilter");
    bool linearFilter = JS_ToBool(ctx, linearFilterVal);

    hnd = SLT_Asset_LoadImage(nameStr, pathStr, linearFilter);

    JS_FreeValue(ctx, path);
    JS_FreeCString(ctx, pathStr);

  } else if (strcmp(assetTypeStr, "sprite") == 0) {
    JSValueConst path = JS_GetPropertyStr(ctx, argv[0], "path");
    const char *pathStr = JS_ToCString(ctx, path);

    // TODO literally any error checking
    int spriteWidth, spriteHeight, marginX, marginY;
    JS_ToInt32(ctx, &spriteWidth, JS_GetPropertyStr(ctx, argv[0], "spriteWidth"));
    JS_ToInt32(ctx, &spriteHeight, JS_GetPropertyStr(ctx, argv[0], "spriteHeight"));
    JS_ToInt32(ctx, &marginX, JS_GetPropertyStr(ctx, argv[0], "marginX"));
    JS_ToInt32(ctx, &marginY, JS_GetPropertyStr(ctx, argv[0], "marginY"));

    hnd = SLT_Asset_LoadSprite(nameStr, pathStr, spriteWidth, spriteHeight, marginX, marginY);

    JS_FreeValue(ctx, path);
    JS_FreeCString(ctx, pathStr);

  } else if (strcmp(assetTypeStr, "speech") == 0) {
    JSValueConst text = JS_GetPropertyStr(ctx, argv[0], "text");
    const char *textStr = JS_ToCString(ctx, text);
    hnd = SLT_Asset_LoadSpeech(nameStr, textStr);

    JS_FreeValue(ctx, text);
    JS_FreeCString(ctx, textStr);

  } else if (strcmp(assetTypeStr, "sound") == 0) {
    JSValueConst path = JS_GetPropertyStr(ctx, argv[0], "path");
    const char *pathStr = JS_ToCString(ctx, path);

    hnd = SLT_Asset_LoadSound(nameStr, pathStr);

    JS_FreeValue(ctx, path);
    JS_FreeCString(ctx, pathStr);

  } else if (strcmp(assetTypeStr, "mod") == 0) {
    JSValueConst path = JS_GetPropertyStr(ctx, argv[0], "path");
    const char *pathStr = JS_ToCString(ctx, path);

    hnd = SLT_Asset_LoadMod(nameStr, pathStr);

    JS_FreeValue(ctx, path);
    JS_FreeCString(ctx, pathStr);

  } else if (strcmp(assetTypeStr, "font") == 0) {
    JSValueConst path = JS_GetPropertyStr(ctx, argv[0], "path");
    const char *pathStr = JS_ToCString(ctx, path);

    hnd = SLT_Asset_LoadFont(nameStr, pathStr);

    JS_FreeValue(ctx, path);
    JS_FreeCString(ctx, pathStr);

  } else if (strcmp(assetTypeStr, "bitmapfont") == 0) {

  } else if (strcmp(assetTypeStr, "tilemap") == 0) {

  } else if (strcmp(assetTypeStr, "canvas") == 0) {

  } else if (strcmp(assetTypeStr, "shader") == 0) {

  } else {
    JS_FreeCString(ctx, nameStr);
    JS_FreeCString(ctx, assetTypeStr);
    return JS_ThrowTypeError(ctx, "Unrecognized options.type");
  }

  JS_FreeCString(ctx, nameStr);
  JS_FreeCString(ctx, assetTypeStr);

  return JS_NewInt32(ctx, hnd);
}

static JSValue js_assets_find(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * name;

  if ((name = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  AssetHandle id = SLT_Asset_Find(name);

  JS_FreeCString(ctx, name);
  return JS_NewInt32(ctx, id);
}

static JSValue js_assets_clearall(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  SLT_Asset_ClearAll();
  return JS_UNDEFINED;
}

static JSValue js_assets_loadini(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * path;

  if ((path = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  SLT_Asset_LoadINI(path);

  JS_FreeCString(ctx, path);
  return JS_UNDEFINED;
}

static JSValue js_assets_textwidth(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int assetHandle;
  const char * str;
  double scale;

  if (JS_ToInt32(ctx, &assetHandle, argv[0])) return JS_EXCEPTION;
  if ((str = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &scale, argv[2])) return JS_EXCEPTION;

  int w = SLT_Asset_TextWidth(assetHandle,str,scale);

  JS_FreeCString(ctx, str);
  return JS_NewInt32(ctx, w);
}

static JSValue js_assets_breakstring(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int width;
  const char * in;

  if (JS_ToInt32(ctx, &width, argv[0])) return JS_EXCEPTION;
  if ((in = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;

  const char *out = SLT_Asset_BreakString(width,in);

  JS_FreeCString(ctx, in);
  return JS_NewString(ctx, out);
}

static JSValue js_assets_imagesize(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int assetHandle;

  if (JS_ToInt32(ctx, &assetHandle, argv[0])) return JS_EXCEPTION;

  const Image *img = SLT_Get_Img(assetHandle);

  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "w", JS_NewInt32(ctx, img->w));
  JS_SetPropertyStr(ctx, obj, "h", JS_NewInt32(ctx, img->h));
  return obj;
}

static const JSCFunctionListEntry js_assets_funcs[] = {
  JS_CFUNC_DEF("load", 1, js_assets_load),
  JS_CFUNC_DEF("find", 1, js_assets_find),
  JS_CFUNC_DEF("clearAll", 0, js_assets_clearall),
  JS_CFUNC_DEF("loadINI", 1, js_assets_loadini),
  JS_CFUNC_DEF("textWidth", 3, js_assets_textwidth),
  JS_CFUNC_DEF("breakString", 2, js_assets_breakstring),
  JS_CFUNC_DEF("imageSize", 1, js_assets_imagesize),
};

static int js_assets_init(JSContext *ctx, JSModuleDef *m)
{    
  JS_SetModuleExportList(ctx, m, js_assets_funcs,
                          countof(js_assets_funcs));
  return 0;
}

JSModuleDef *js_init_module_assets(JSContext *ctx, const char *module_name)
{
  JSModuleDef *m;
  m = JS_NewCModule(ctx, module_name, js_assets_init);
  if (!m)
      return NULL;
  JS_AddModuleExportList(ctx, m, js_assets_funcs, countof(js_assets_funcs));
  return m;
}