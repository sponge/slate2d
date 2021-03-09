#include "../src/slate2d.h"
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

static JSValue js_assets_create(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (!JS_IsObject(argv[0])) return JS_ThrowTypeError(ctx, "options arg is not an object");

  JSValueConst assetType = JS_GetPropertyStr(ctx, argv[0], "type");
  const char *assetTypeStr = JS_ToCString(ctx, assetType);
  JS_FreeValue(ctx, assetType);

  if (assetTypeStr == nullptr) {
    return JS_ThrowTypeError(ctx, "options.type is missing or not a string");
  }

  if (strcmp(assetTypeStr, "image") == 0) {

  } else if (strcmp(assetTypeStr, "sprite") == 0) {

  } else if (strcmp(assetTypeStr, "speech") == 0) {
    JSValueConst text = JS_GetPropertyStr(ctx, argv[0], "text");
    const char *textStr = JS_ToCString(ctx, text);
    JS_FreeValue(ctx, text);
    JS_FreeCString(ctx, textStr);
    //SLT_Asset_Create(ASSET_SPEECH, )
  } else if (strcmp(assetTypeStr, "sound") == 0) {

  } else if (strcmp(assetTypeStr, "mod") == 0) {

  } else if (strcmp(assetTypeStr, "font") == 0) {

  } else if (strcmp(assetTypeStr, "bitmapfont") == 0) {

  } else if (strcmp(assetTypeStr, "canvas") == 0) {

  } else if (strcmp(assetTypeStr, "shader") == 0) {

  } else {
    // error
  }
  JS_FreeCString(ctx, assetTypeStr);

  // FIXME: implement me
  // SLT_Asset_Create();

  return JS_UNDEFINED;
}

static JSValue js_assets_find(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * name;

  if ((name = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  AssetHandle id = SLT_Asset_Find(name);

  JS_FreeCString(ctx, name);
  return JS_NewInt32(ctx, id);
}

static JSValue js_assets_load(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int assetHandle;

  if (JS_ToInt32(ctx, &assetHandle, argv[0])) return JS_EXCEPTION;

  SLT_Asset_Load(assetHandle);
  return JS_UNDEFINED;
}

static JSValue js_assets_loadall(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  SLT_Asset_LoadAll();
  return JS_UNDEFINED;
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
  JS_CFUNC_DEF("create", 1, js_assets_create),
  JS_CFUNC_DEF("find", 1, js_assets_find),
  JS_CFUNC_DEF("load", 1, js_assets_load),
  JS_CFUNC_DEF("loadAll", 0, js_assets_loadall),
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