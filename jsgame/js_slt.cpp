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

void RenderObjRecursively(JSContext *ctx, JSValue obj, const char *nodeName) {
  JSPropertyEnum *tab_atom;
  uint32_t tab_atom_count;

  ImGui::PushID(obj.u.ptr);

  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::AlignTextToFramePadding();
  bool node_open = ImGui::TreeNode(nodeName, "%s", nodeName);
  ImGui::TableSetColumnIndex(1);
  const char *nodeValStr = JS_ToCString(ctx, obj);
  ImGui::Text("%s", nodeValStr);
  JS_FreeCString(ctx, nodeValStr);

  if (node_open) {
    if (JS_GetOwnPropertyNames(ctx, &tab_atom, &tab_atom_count, obj, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK)) {
      ImGui::TreePop();
      ImGui::PopID();
      return;
    }

    for (int i = 0; i < tab_atom_count; i++) {
      JSValue val = JS_GetProperty(ctx, obj, tab_atom[i].atom);
      //JSValue variable_json = js_debugger_get_variable(ctx, state, JS_AtomToString(ctx, tab_atom[i].atom), value);
      const char *keyStr = JS_AtomToCString(ctx, tab_atom[i].atom);

      if (JS_IsObject(val)) {
        RenderObjRecursively(ctx, val, keyStr);
      } else {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(keyStr, flags, "%s", keyStr);

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);

        const char *valStr = JS_ToCString(ctx, val);
        ImGui::Text("%s", valStr);

        JS_FreeCString(ctx, valStr);
      }
      JS_FreeValue(ctx, val);
      JS_FreeCString(ctx, keyStr);

    }

    for(uint32_t i = 0; i < tab_atom_count; i++) {
      JS_FreeAtom(ctx, tab_atom[i].atom);
    }

    js_free(ctx, tab_atom);

    ImGui::TreePop();
  }
  ImGui::PopID();
}

static JSValue js_slt_showobj(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  if (!ImGui::Begin("Object Inspector")) {
      ImGui::End();
      return JS_UNDEFINED;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
  if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {
    const char *title = JS_ToCString(ctx, argv[0]);
    RenderObjRecursively(ctx, argv[1], title);
    JS_FreeCString(ctx, title);
    ImGui::EndTable();
  }
  
  ImGui::PopStyleVar();
  ImGui::End();

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

  unsigned int playHandle = SLT_Snd_Play(asset, volume, pan, loop);
  return JS_NewUint32(ctx, playHandle);
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

  bool pressed = SLT_In_ButtonPressed(buttonNum, delay, repeat);
  return JS_NewBool(ctx, pressed);
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

static JSValue js_slt_readfile(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
  const char *path = JS_ToCString(ctx, argv[0]);

  char *buffer;
  if (SLT_FS_ReadFile(path, (void **)&buffer) == -1) {
    JS_FreeCString(ctx, path);
    return JS_ThrowTypeError(ctx, "Couldn't read file");
  }

  JS_FreeCString(ctx, path);
  JSValue str = JS_NewString(ctx, buffer);
  free(buffer);

  return str;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-designator"
static const JSCFunctionListEntry js_slt_funcs[] = {
  JS_CFUNC_DEF("printWin", 1, js_slt_printwin),
  JS_CFUNC_DEF("showObj", 2, js_slt_showobj),
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
  JS_CFUNC_DEF("readFile", 1, js_slt_readfile),
};
#pragma clang diagnostic pop

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