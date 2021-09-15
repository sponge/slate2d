#include "../src/slate2d.h"
#include <imgui.h>
#include <string>
extern "C" {
#include <cutils.h>
#include <quickjs.h>
}

static JSValue js_slt_printwin(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
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

void RenderValue(JSContext *ctx, JSValue obj, JSAtom prop, const char *titleOverride = "")
{
  ImGui::TableNextRow();
  ImGui::TableSetColumnIndex(0);
  ImGui::AlignTextToFramePadding();

  JSValue val = prop == 0 ? obj : JS_GetProperty(ctx, obj, prop);

  // title is either the property name or override
  bool node_open = false;
  if (prop == 0) {
    if (JS_IsObject(obj)) {
      node_open = ImGui::TreeNode(JS_VALUE_GET_PTR(val), "%s", titleOverride);
    }
    else {
      ImGui::Text("%s", titleOverride);
    }
  }
  else {
    const char *propStr = JS_AtomToCString(ctx, prop);
    if (JS_IsObject(val)) {
      node_open = ImGui::TreeNode(JS_VALUE_GET_PTR(val), "%s", propStr);
    }
    else {
      ImGui::Text("%s", propStr);
    }
    JS_FreeCString(ctx, propStr);
  }

  // value is usually .toString unless its an array
  ImGui::TableSetColumnIndex(1);
  ImGui::SetNextItemWidth(-FLT_MIN);

  const char *valStr;
  // some arrays are big so maybe don't print all the values?
  std::string out = "";
  if (JS_IsArray(ctx, val)) {
    JSValueConst length = JS_GetPropertyStr(ctx, val, "length");
    uint32_t len;
    JS_ToUint32(ctx, &len, length);
    JS_FreeValue(ctx, length);

    out += "[" + std::to_string(len) + "] ";

    uint32_t loopLen = len > 10 ? 10 : len;
    for (uint32_t i = 0; i < loopLen; i++) {
      JSValueConst arrVal = JS_GetPropertyUint32(ctx, val, i);
      JSValueConst strVal = JS_ToString(ctx, arrVal);
      const char *str = JS_ToCString(ctx, strVal);
      out += str;
      if (i + 1 < loopLen) out += ",";
      JS_FreeValue(ctx, arrVal);
      JS_FreeValue(ctx, strVal);
      JS_FreeCString(ctx, str);
    }

    if (loopLen < len) out += " ...";
    valStr = out.c_str();
  }
  else {
    valStr = JS_ToCString(ctx, val);
  }

  if (prop == 0) {
    ImGui::Text("%s", valStr);
  }
  else {
    ImGui::PushID(prop);
    ImGui::Selectable(valStr);
    ImGui::PopID();
  }

  if (!JS_IsArray(ctx, val)) {
    JS_FreeCString(ctx, valStr);
  }

  // editor
  static char evalStr[1024];
  const char *propStr = JS_AtomToCString(ctx, prop);
  if (prop != 0 && ImGui::BeginPopupContextItem(propStr, ImGuiPopupFlags_MouseButtonLeft)) {
    if (ImGui::IsWindowAppearing()) {
      ImGui::SetKeyboardFocusHere();
      evalStr[0] = '\0';
    }

    if (ImGui::InputText("New value", evalStr, IM_ARRAYSIZE(evalStr), ImGuiInputTextFlags_EnterReturnsTrue)) {
      JSValue eval = JS_Eval(ctx, evalStr, strlen(evalStr), "<eval>", 0);
      if (!JS_IsException(eval)) {
        JSValue dup = JS_DupValue(ctx, eval);
        JS_SetProperty(ctx, obj, prop, dup);
      }
      JS_FreeValue(ctx, eval);
      ImGui::CloseCurrentPopup();
    }

    if (ImGui::Selectable("Save ref to globalThis.temp")) {
      JSValue global = JS_GetGlobalObject(ctx);
      JSValue dup = JS_DupValue(ctx, val);
      JS_SetPropertyStr(ctx, global, "temp", dup);
      JS_FreeValue(ctx, global);
      ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  JS_FreeCString(ctx, propStr);

  // children
  if (node_open) {
    JSPropertyEnum *tab_atom;
    uint32_t tab_atom_count;

    if (JS_GetOwnPropertyNames(ctx, &tab_atom, &tab_atom_count, val, JS_GPN_STRING_MASK | JS_GPN_SYMBOL_MASK)) {
      ImGui::TreePop();
      ImGui::PopID();
      return;
    }

    for (int i = 0; i < tab_atom_count; i++) {
      RenderValue(ctx, val, tab_atom[i].atom);
      JS_FreeAtom(ctx, tab_atom[i].atom);
    }

    js_free(ctx, tab_atom);

    ImGui::TreePop();
  }

  if (prop != 0) {
    JS_FreeValue(ctx, val);
  }
}

static JSValue js_slt_showobj(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  if (!ImGui::Begin("Object Inspector")) {
    ImGui::End();
    return JS_UNDEFINED;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
  if (ImGui::BeginTable("split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable)) {
    const char *title = JS_ToCString(ctx, argv[0]);
    RenderValue(ctx, argv[1], 0, title);
    JS_FreeCString(ctx, title);
    ImGui::EndTable();
  }

  ImGui::PopStyleVar();
  ImGui::End();

  return JS_UNDEFINED;
}

static JSValue js_slt_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int level;
  const char *error;

  if (JS_ToInt32(ctx, &level, argv[0])) return JS_EXCEPTION;
  if ((error = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;

  SLT_Error(level, error);

  JS_FreeCString(ctx, error);
  return JS_UNDEFINED;
}

static JSValue js_slt_console(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  const char *text;

  if ((text = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

  SLT_SendConsoleCommand(text);

  JS_FreeCString(ctx, text);
  return JS_UNDEFINED;
}

static JSValue js_slt_sndplay(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int asset, loop;
  double volume, pan;

  if (JS_ToInt32(ctx, &asset, argv[0])) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &volume, argv[1])) return JS_EXCEPTION;
  if (JS_ToFloat64(ctx, &pan, argv[2])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &loop, argv[3])) return JS_EXCEPTION;

  unsigned int playHandle = SLT_Snd_Play(asset, volume, pan, loop);
  return JS_NewUint32(ctx, playHandle);
}

static JSValue js_slt_sndstop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int handle;

  if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;

  SLT_Snd_Stop(handle);
  return JS_UNDEFINED;
}

static JSValue js_slt_sndpauseresume(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int handle, paused;

  if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &paused, argv[1])) return JS_EXCEPTION;

  SLT_Snd_PauseResume(handle, paused);
  return JS_UNDEFINED;
}

static JSValue js_slt_registerbuttons(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  if (!JS_IsArray(ctx, argv[0])) return JS_EXCEPTION;
  int len = 0;
  JSValueConst lenVal = JS_GetPropertyStr(ctx, argv[0], "length");
  JS_ToInt32(ctx, &len, lenVal);
  JS_FreeValue(ctx, lenVal);
  const char **strings = (const char **)malloc(sizeof(void *) * len);
  for (int i = 0; i < len; i++) {
    JSValueConst val = JS_GetPropertyUint32(ctx, argv[0], i);
    strings[i] = JS_ToCString(ctx, val);
    JS_FreeValue(ctx, val);
  }
  SLT_In_AllocateButtons(&strings[0], len);

  for (int i = 0; i < len; i++) {
    JS_FreeCString(ctx, strings[i]);
  }
  free((void *)strings);
  return JS_UNDEFINED;
}

static JSValue js_slt_buttonpressed(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int buttonNum, delay, repeat;

  if (JS_ToInt32(ctx, &buttonNum, argv[0])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &delay, argv[1])) return JS_EXCEPTION;
  if (JS_ToInt32(ctx, &repeat, argv[2])) return JS_EXCEPTION;

  bool pressed = SLT_In_ButtonPressed(buttonNum, delay, repeat);
  return JS_NewBool(ctx, pressed);
}

static JSValue js_slt_setwindowtitle(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  const char *title;

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

static JSValue js_slt_getmouse(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  MousePosition pos = SLT_In_MousePosition();
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, pos.x));
  JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, pos.y));

  return obj;
}

static JSValue js_slt_getcontrolleranalog(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int num;
  if (JS_ToInt32(ctx, &num, argv[0])) {
    return JS_ThrowTypeError(ctx, "No controller index passed");
  }

  AnalogAxes axes = SLT_In_ControllerAnalog(num);
  JSValue arr = JS_NewArray(ctx);
  JS_SetPropertyInt64(ctx, arr, 0, JS_NewFloat64(ctx, axes.leftX));
  JS_SetPropertyInt64(ctx, arr, 1, JS_NewFloat64(ctx, axes.leftY));
  JS_SetPropertyInt64(ctx, arr, 2, JS_NewFloat64(ctx, axes.rightX));
  JS_SetPropertyInt64(ctx, arr, 3, JS_NewFloat64(ctx, axes.rightY));
  JS_SetPropertyInt64(ctx, arr, 4, JS_NewFloat64(ctx, axes.triggerLeft));
  JS_SetPropertyInt64(ctx, arr, 5, JS_NewFloat64(ctx, axes.triggerRight));

  return arr;
}

static JSValue js_slt_getresolution(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  Dimensions res = SLT_GetResolution();
  JSValue obj = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, obj, "w", JS_NewInt32(ctx, res.w));
  JS_SetPropertyStr(ctx, obj, "h", JS_NewInt32(ctx, res.h));

  return obj;
}

static JSValue js_slt_readfile(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
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

static JSValue js_slt_writefile(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  const char *path = JS_ToCString(ctx, argv[0]);
  const char *contents = JS_ToCString(ctx, argv[1]);

  int ret = SLT_FS_WriteFile(path, contents, strlen(contents));
  JS_FreeCString(ctx, path);
  JS_FreeCString(ctx, contents);

  return JS_NewInt32(ctx, ret);
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
  JS_CFUNC_DEF("controllerAnalog", 1, js_slt_getcontrolleranalog),
  JS_CFUNC_DEF("resolution", 0, js_slt_getresolution),
  JS_CFUNC_DEF("readFile", 1, js_slt_readfile),
  JS_CFUNC_DEF("writeFile", 2, js_slt_writefile),

};
#pragma clang diagnostic pop

static int js_slt_init(JSContext *ctx, JSModuleDef *m)
{
  JS_SetModuleExportList(ctx, m, js_slt_funcs, countof(js_slt_funcs));
  return 0;
}

static JSValue js_slt_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
  int i;
  const char *str;
  size_t len;

  std::string out = "";
  for (i = 0; i < argc; i++) {
    if (i != 0) out += " ";
    str = JS_ToCStringLen(ctx, &len, argv[i]);
    if (!str) return JS_EXCEPTION;
    out += str;
    JS_FreeCString(ctx, str);
  }
  out += "\n";
  SLT_Print(out.c_str());

  return JS_UNDEFINED;
}

JSModuleDef *js_init_module_slt(JSContext *ctx, const char *module_name)
{
  JSModuleDef *m;
  m = JS_NewCModule(ctx, module_name, js_slt_init);
  if (!m) return NULL;
  JS_AddModuleExportList(ctx, m, js_slt_funcs, countof(js_slt_funcs));

  JSValue global_obj, console;
  global_obj = JS_GetGlobalObject(ctx);
  console = JS_NewObject(ctx);
  JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, js_slt_console_log, "log", 1));
  JS_SetPropertyStr(ctx, global_obj, "console", console);
  JS_FreeValue(ctx, global_obj);

  return m;
}