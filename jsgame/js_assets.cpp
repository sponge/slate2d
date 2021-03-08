#include "../src/slate2d.h"
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

static JSValue js_assets_create(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  // FIXME: implement me
  // SLT_Asset_Create();
  return JS_UNDEFINED;
}

static JSValue js_assets_find(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * name;

  if ((name = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  SLT_Asset_Find(name);
  return JS_UNDEFINED;
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
  return JS_UNDEFINED;
}

static JSValue js_assets_textwidth(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int assetHandle;
  const char * str;
  double scale;

  if (JS_ToInt32(ctx, &assetHandle, argv[0])) return JS_EXCEPTION;
  if ((str = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &scale, argv[2])) return JS_EXCEPTION;

  SLT_Asset_TextWidth(assetHandle,str,scale);
  return JS_UNDEFINED;
}

static JSValue js_assets_breakstring(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int width;
  const char * in;

  if (JS_ToInt32(ctx, &width, argv[0])) return JS_EXCEPTION;
  if ((in = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;

  SLT_Asset_BreakString(width,in);
  return JS_UNDEFINED;
}

static JSValue js_assets_imagesize(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int assetHandle;

  if (JS_ToInt32(ctx, &assetHandle, argv[0])) return JS_EXCEPTION;

  SLT_Get_Img(assetHandle);
  return JS_UNDEFINED;
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