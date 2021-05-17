#include "../src/slate2d.h"
#include <string>
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

namespace QJS {
  class Value {
  private:
    const char *cStr = nullptr;
  public:
    JSContext *ctx;
    JSValueConst val;
    bool noFree = false;

    Value(JSContext *ctx, JSValueConst val, bool noFree = false) : ctx(ctx), val(val), noFree(noFree) {
      if (isString()) {
        cStr = JS_ToCString(ctx, val);
      }
    }

    ~Value() {
      if (!noFree) JS_FreeValue(ctx, val);
      if (cStr != nullptr) JS_FreeCString(ctx, cStr);
    }

    operator bool () const {
      return !JS_IsUndefined(val) && !JS_IsNull(val);
    }

    const Value operator[](std::string str) const {
      JSValueConst ret = JS_GetPropertyStr(ctx, val, str.c_str());
      return Value(ctx, ret);
    }

    const Value operator[](const char *str) const {
      JSValueConst ret = JS_GetPropertyStr(ctx, val, str);
      return Value(ctx, ret);
    }

    bool isString() const {
      return JS_IsString(val);
    }

    bool isObject() const {
      return JS_IsObject(val);
    }

    bool isUndefined() const {
      return JS_IsUndefined(val);
    }

    // note: returns string "undefined" if value is undefined
    // use isString or asCString != nullptr to check if value is a string
    std::string asString() const {
      return JS_ToCString(ctx, val);
    }

    const char * asCString() const {
      return cStr;
    }
  };
}

// FIXME: probably needs better checking of types on invalid/missing properties.
// FIXME: generalize some of the getters, check https://github.com/PetterS/quickjs/blob/master/module.c for inspiration
static JSValue js_assets_load(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto assetSettings = QJS::Value(ctx, argv[0], true);

  if (!assetSettings.isObject()) {
    return JS_ThrowTypeError(ctx, "options arg is not an object");
  }

  auto assetTypeStr = assetSettings["type"].asCString();
  if (!assetTypeStr) {
    return JS_ThrowTypeError(ctx, "options.type is missing or not a string");
  }

  const char *nameStr = assetSettings["name"].asCString();
  if (!nameStr) {
    return JS_ThrowTypeError(ctx, "options.name is missing or not a string");
  }

  AssetHandle hnd;

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

    int sprWidth, sprHeight, marginX, marginY;
    JSValueConst sprWidthVal = JS_GetPropertyStr(ctx, argv[0], "spriteWidth");
    if (JS_ToInt32(ctx, &sprWidth, sprWidthVal)) return JS_EXCEPTION;

    JSValueConst sprHeightVal = JS_GetPropertyStr(ctx, argv[0], "spriteHeight");
    if (JS_ToInt32(ctx, &sprHeight, sprHeightVal)) return JS_EXCEPTION;

    JSValueConst marginXVal = JS_GetPropertyStr(ctx, argv[0], "marginX");
    if (JS_ToInt32(ctx, &marginX, marginXVal)) return JS_EXCEPTION;

    JSValueConst marginYVal = JS_GetPropertyStr(ctx, argv[0], "marginY");
    if (JS_ToInt32(ctx, &marginY, marginYVal)) return JS_EXCEPTION;

    hnd = SLT_Asset_LoadSprite(nameStr, pathStr, sprWidth, sprHeight, marginX, marginY);

    // FIXME: this won't get hit on return early
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