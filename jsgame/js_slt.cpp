#include "../src/slate2d.h"
#include <imgui.h>
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

static JSValue js_slt_printwin(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  using namespace ImGui;
  const char *title, *key, *value;

  if ((title = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;
  if ((key = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;
  if ((value = JS_ToCString(ctx, argv[2])) == NULL) return JS_EXCEPTION;

	SetNextWindowSize(ImVec2(250, 500), ImGuiCond_FirstUseEver);
	Begin(title, nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoFocusOnAppearing);

	float width = GetWindowContentRegionWidth();
	float keyWidth = CalcTextSize(key).x;
	float valWidth = CalcTextSize(value).x;

	Text("%s", key);
	if (keyWidth + valWidth + 20 < width) {
    SameLine();
	}
	int x = (int)(width - valWidth);
	x = x < 5 ? 5 : x;
	SetCursorPosX((float)x);
	Text("%s", value);
	Separator();
	End();

  JS_FreeCString(ctx, title);
  JS_FreeCString(ctx, key);
  JS_FreeCString(ctx, value);
  return JS_UNDEFINED;
}

static JSValue js_slt_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int level;
  const char * error;

  if (JS_ToInt32(ctx, &level, argv[0])) return JS_EXCEPTION;
  if ((error = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;

  SLT_Error(level,error);

  JS_FreeCString(ctx, error);
  return JS_UNDEFINED;
}

static JSValue js_slt_console(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * text;

  if ((text = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  SLT_SendConsoleCommand(text);

  JS_FreeCString(ctx, text);
  return JS_UNDEFINED;
}

static JSValue js_slt_sndplay(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int asset, loop;
  double volume, pan;

  if (JS_ToInt32(ctx, &asset, argv[0])) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &volume, argv[1])) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &pan, argv[2])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &loop, argv[3])) return JS_EXCEPTION;

}

static JSValue js_slt_sndstop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int handle;

  if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;

  SLT_Snd_Stop(handle);
  return JS_UNDEFINED;
}

static JSValue js_slt_sndpauseresume(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int handle, paused;

  if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &paused, argv[1])) return JS_EXCEPTION;

  SLT_Snd_PauseResume(handle,paused);
  return JS_UNDEFINED;
}

static JSValue js_slt_registerbuttons(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (!JS_IsArray(ctx, argv[0])) return JS_EXCEPTION;
  int len = 0;
  JSValueConst lenVal = JS_GetPropertyStr(ctx, argv[0], "length");
  JS_ToInt32(ctx, &len, lenVal);
  JS_FreeValue(ctx, lenVal);
  const char **strings = (const char **)malloc(sizeof(void*) * len);
  for (int i = 0; i < len; i++) {
    JSValueConst val = JS_GetPropertyUint32(ctx, argv[0], i);
    strings[i] = JS_ToCString(ctx, val);
    JS_FreeValue(ctx, val);
  }
  SLT_In_AllocateButtons(&strings[0], len);

  for (int i = 0; i < len; i++) {
    JS_FreeCString(ctx, strings[i]);
  }
  free((void*)strings);
  return JS_UNDEFINED;
}

static JSValue js_slt_buttonpressed(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int buttonNum, delay, repeat;

  if (JS_ToInt32(ctx, &buttonNum, argv[0])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &delay, argv[1])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &repeat, argv[2])) return JS_EXCEPTION;
}

static JSValue js_slt_setwindowtitle(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char * title;

  if ((title = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  SLT_SetWindowTitle(title);

  JS_FreeCString(ctx, title);
  return JS_UNDEFINED;
}

#if defined(_WIN32)
static const char *platform = "windows";
#elif defined(MACOS)
static const char *platform = "macos";
#elif defined(__EMSCRIPTEN__)
static const char *platform = "emscripten";
#else
static const char *platform = "unknown";
#endif

static JSValue js_slt_getmouse(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  MousePosition pos = SLT_In_MousePosition();
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, pos.x));
  JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, pos.y));

  return obj;
}

static JSValue js_slt_getresolution(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  int w, h; 
  SLT_GetResolution(&w, &h);
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "w", JS_NewInt32(ctx, w));
  JS_SetPropertyStr(ctx, obj, "h", JS_NewInt32(ctx, h));

  return obj;
}

static const JSCFunctionListEntry js_slt_funcs[] = {
  JS_CFUNC_DEF("printWin", 1, js_slt_printwin),
  JS_CFUNC_DEF("error", 2, js_slt_error),
  JS_CFUNC_DEF("console", 1, js_slt_console),
  JS_CFUNC_DEF("sndPlay", 4, js_slt_sndplay),
  JS_CFUNC_DEF("sndStop", 1, js_slt_sndstop),
  JS_CFUNC_DEF("sndPauseResume", 2, js_slt_sndpauseresume),
  JS_CFUNC_DEF("registerButtons", 1, js_slt_registerbuttons),
  JS_CFUNC_DEF("buttonPressed", 3, js_slt_buttonpressed),
  JS_CFUNC_DEF("setWindowTitle", 1, js_slt_setwindowtitle),
  JS_PROP_STRING_DEF("platform", platform, 0),
  JS_CFUNC_DEF("mouse", 0, js_slt_getmouse),
  JS_CFUNC_DEF("resolution", 0, js_slt_getresolution),
};

static int js_slt_init(JSContext *ctx, JSModuleDef *m) {    
  JS_SetModuleExportList(ctx, m, js_slt_funcs,
                          countof(js_slt_funcs));
  return 0;
}

JSModuleDef *js_init_module_slt(JSContext *ctx, const char *module_name) {
  JSModuleDef *m;
  m = JS_NewCModule(ctx, module_name, js_slt_init);
  if (!m)
    return NULL;
  JS_AddModuleExportList(ctx, m, js_slt_funcs, countof(js_slt_funcs));
  return m;
}