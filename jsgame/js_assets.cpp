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
    std::string str;
  public:
    JSContext *ctx;
    JSValueConst val;
    bool noFree = false;

    Value(JSContext *ctx, JSValueConst val, bool noFree = false) : ctx(ctx), val(val), noFree(noFree) {
        cStr = isString() ? JS_ToCString(ctx, val) : nullptr;
    }

    ~Value() {
      if (!noFree) JS_FreeValue(ctx, val);
      if (cStr != nullptr) JS_FreeCString(ctx, cStr);
    }

    operator bool() const {
      return JS_ToBool(ctx, val);
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

    bool isNumber() const {
      return JS_IsNumber(val);
    }

    bool isUndefined() const {
      return JS_IsUndefined(val);
    }

    // note: returns string "undefined" if value is undefined
    // use isString or asCString != nullptr to check if value is a string
    const std::string asString() const {
        const char *tempStr = JS_ToCString(ctx, val);
        std::string str = tempStr;
        JS_FreeCString(ctx, tempStr);

        return str;
    }

    // note: returns null if not a string.
    // asString will return the string "undefined" like it does in JS
    const char * asCString() const {
      return cStr;
    }

    int32_t asInt32() const {
      int32_t ret;
      JS_ToInt32(ctx, &ret, val);
      return ret;
    }
  };
}

#define CHECKSTR(key) assetSettings[key].asCString(); if (!assetSettings[key].asCString()) return JS_ThrowTypeError(ctx, "options." key "is not a string");
#define CHECKINT32(key) assetSettings[key].asInt32(); if (!assetSettings[key].isNumber()) return JS_ThrowTypeError(ctx, "options." key "is not an int");

static JSValue js_assets_load(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  auto assetSettings = QJS::Value(ctx, argv[0], true);

  if (!assetSettings.isObject()) {
    return JS_ThrowTypeError(ctx, "options arg is not an object");
  }

  if (!assetSettings["type"].isString()) {
    return JS_ThrowTypeError(ctx, "options.type is missing or not a string");
  }

  const char *name = CHECKSTR("name")

  AssetHandle hnd;

  auto assetType = assetSettings["type"].asString();
  if (assetType == "image") {
    const char *path = CHECKSTR("path")
    bool linearFilter = assetSettings["linearFilter"];
    hnd = SLT_Asset_LoadImage(name, path, linearFilter);

  } else if (assetType == "sprite") {
    const char *path = CHECKSTR("path")
    int32_t sprWidth = CHECKINT32("spriteWidth")
    int32_t sprHeight = CHECKINT32("spriteHeight")
    int32_t marginX = CHECKINT32("marginX")
    int32_t marginY = CHECKINT32("marginY")
    hnd = SLT_Asset_LoadSprite(name, path, sprWidth, sprHeight, marginX, marginY);

  } else if (assetType == "speech") {
    const char *text = CHECKSTR("text")
    hnd = SLT_Asset_LoadSpeech(name, text);

  } else if (assetType == "sound") {
    const char *path = CHECKSTR("path")
    hnd = SLT_Asset_LoadSound(name, path);

  } else if (assetType == "mod") {
    const char *path = CHECKSTR("path")
    hnd = SLT_Asset_LoadMod(name, path);

  } else if (assetType == "font") {
    const char *path = CHECKSTR("path")
    hnd = SLT_Asset_LoadFont(name, path);

  } else if (assetType == "bitmapfont") {
    const char *path = CHECKSTR("path")
    const char *glyphs = CHECKSTR("glyphs");
    int32_t glyphWidth = CHECKINT32("glyphWidth")
    int32_t charSpacing = CHECKINT32("charSpacing")
    int32_t spaceWidth = CHECKINT32("spaceWidth")
    int32_t lineHeight = CHECKINT32("lineHeight")
    hnd = SLT_Asset_LoadBitmapFont(name, path, glyphs, glyphWidth, charSpacing, spaceWidth, lineHeight);

  } else if (assetType == "canvas") {
    int32_t width = CHECKINT32("width")
    int32_t height = CHECKINT32("height")
    hnd = SLT_Asset_LoadCanvas(name, width, height);

  } else if (assetType ==  "shader") {
    bool isFile = assetSettings["isFile"];
    const char *vs = CHECKSTR("vs");
    const char *fs = CHECKSTR("fs");
    hnd = SLT_Asset_LoadShader(name, isFile, vs, fs);

  } else {
    return JS_ThrowTypeError(ctx, "Unrecognized options.type");
  }

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