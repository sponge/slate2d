#include "../src/slate2d.h"
#include "js_libs.h"
#include "timing.h"
#include <imgui.h>
#include <string>
extern "C" {
#include <quickjs-debugger.h>
#include <quickjs-libc.h>
#include <quickjs.h>
}

JSModuleDef *physfs_module_loader(JSContext *ctx, const char *module_name, void *opaque)
{
  // nasty hacky code ahead.
  // use main.js because it will return pak00.pk3 when running out of a pk3 file. this probably breaks cases
  // where you have js files in multiple pk3s though but i don't care about that right now. maybe we just need
  // to chop off (file).pk3 if it is in the end of the path?
  const char *root = SLT_FS_RealDir("/js/main.js");

  // if module_name resolves to the full path, chop off the prefix to give us the virtual path
  // relative to where main.js is since physicsfs doesn't support ./ or ../ at all
  const char *base = strstr(module_name, root);
  const char *virtualPath;
  if (base != nullptr) {
    virtualPath = base + strlen(root);
  }
  else {
    virtualPath = module_name;
  }

  // virtualPath should now be something like /test/testmodule.js
  char *script = nullptr;
  int sz = SLT_FS_ReadFile(virtualPath, (void **)&script);

  if (sz <= 0) {
    JS_ThrowReferenceError(ctx, "could not load module filename '%s'", module_name);
    return NULL;
  }

  // compile the module here. realdir is used because if we're running from the filesystem, it'll
  // give full filesystem paths that the debugger will use.
  const char *realdir = SLT_FS_RealDir(module_name);
  std::string fullpath = realdir == nullptr ? module_name : std::string(realdir) + "/" + std::string(module_name);
  JSValue func_val =
    JS_Eval(ctx, (char *)script, sz, fullpath.c_str(), JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
  free(script);

  if (JS_IsException(func_val)) return NULL;
  /* XXX: could propagate the exception */
  js_module_set_import_meta(ctx, func_val, true, false);
  /* the module is already referenced, so we must free it */
  JSModuleDef *m = (JSModuleDef *)JS_VALUE_GET_PTR(func_val);
  JS_FreeValue(ctx, func_val);

  return m;
}

class SLTJSInstance {
public:
  JSContext *ctx;
  JSRuntime *rt;
  JSValueConst global;
  JSValueConst module;
  JSValueConst main;
  bool moduleIsClass;

  JSValueConst updateFunc;
  JSValueConst drawFunc;
  JSValueConst startFunc;
  JSValueConst saveFunc;

  SLTJSInstance()
  {
    rt = JS_NewRuntime();
    // JS_SetMaxStackSize(rt, 1048576);
    ctx = JS_NewContext(rt);
    global = JS_GetGlobalObject(ctx);

    js_init_module_draw(ctx, "draw");
    js_init_module_slt(ctx, "slate2d");
    js_init_module_assets(ctx, "assets");

    JS_SetModuleLoaderFunc(rt, nullptr, physfs_module_loader, nullptr);
  }

  ~SLTJSInstance()
  {
    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, startFunc);
    JS_FreeValue(ctx, updateFunc);
    JS_FreeValue(ctx, drawFunc);
    JS_FreeValue(ctx, saveFunc);
    JS_FreeValue(ctx, main);
    JS_FreeValue(ctx, module);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
  }

  bool Init()
  {
    const char *import = "import main from './js/main.js'; globalThis.mainModule = main;";

    JSValue imported = JS_Eval(ctx, import, strlen(import), "<import>", JS_EVAL_TYPE_MODULE);
    if (JS_IsException(imported)) {
      JS_FreeValue(ctx, imported);
      Error("Could not eval main.js");
      return false;
    }

    module = JS_GetPropertyStr(ctx, global, "mainModule");
    if (JS_IsFunction(ctx, module)) {
      moduleIsClass = true;
    }
    else if (JS_IsObject(module)) {
      moduleIsClass = false;
    }
    else {
      Error("main.js did not export a function or object.");
      return false;
    }

    return true;
  }

  bool CallStart(const char *state)
  {
    JSValueConst jsState = state == nullptr || strlen(state) == 0 ? JS_UNDEFINED : JS_NewString(ctx, state);

    if (moduleIsClass) {
      JS_SetPropertyStr(ctx, global, "initialState", jsState);
      const char *str = "globalThis.main = new globalThis.mainModule(globalThis.initialState);";
      JS_Eval(ctx, str, strlen(str), "<import>", JS_EVAL_TYPE_MODULE);
      main = JS_GetPropertyStr(ctx, global, "main");

      if (!JS_IsObject(main)) {
        Error("Main class constructor did not return an object");
        return false;
      }
    }
    else {
      main = module;
      JS_SetPropertyStr(ctx, global, "main", main);
    }

    if (JS_IsException(main)) {
      Error("Caught exception while initializing class.");
      return false;
    }

    startFunc = JS_GetPropertyStr(ctx, main, "start");
    updateFunc = JS_GetPropertyStr(ctx, main, "update");
    drawFunc = JS_GetPropertyStr(ctx, main, "draw");
    saveFunc = JS_GetPropertyStr(ctx, main, "save");

    if (!JS_IsFunction(ctx, updateFunc)) {
      Error("Module did not export an update function.");
      return false;
    }

    if (!JS_IsFunction(ctx, drawFunc)) {
      Error("Module did not export a draw function.");
      return false;
    }

    if (!moduleIsClass) {
      if (!JS_IsFunction(ctx, startFunc)) {
        Error("Module did not export a start function.");
        return false;
      }

      JSValue jsResult = JS_Call(ctx, startFunc, main, 1, &jsState);
      if (JS_IsException(jsResult)) {
        Error("Caught exception while calling start function.");
        return false;
      }

      JS_FreeValue(ctx, jsResult);
    }

    return true;
  }

  bool CallUpdate(float dt) const
  {
    JSValue jsResult;

    JSValueConst arg = JS_NewFloat64(ctx, dt);
    jsResult = JS_Call(ctx, updateFunc, main, 1, &arg);
    if (JS_IsException(jsResult)) {
      return false;
    }

    JS_FreeValue(ctx, arg);
    JS_FreeValue(ctx, jsResult);

    return true;
  }

  bool CallDraw() const
  {
    JSValue jsResult = JS_Call(ctx, drawFunc, main, 0, nullptr);
    if (JS_IsException(jsResult)) {
      return false;
    }

    JS_FreeValue(ctx, jsResult);
    return true;
  }

  std::string CallSave() const
  {
    if (!JS_IsFunction(ctx, saveFunc)) {
      return "";
    }

    JSValue saveResult = JS_Call(ctx, saveFunc, main, 0, nullptr);
    if (JS_IsException(saveResult)) {
      Error("Caught exception in Save()");
      return "";
    }

    if (!JS_IsString(saveResult)) {
      JS_FreeValue(ctx, saveResult);
      return "";
    }

    const char *tempStr = JS_ToCString(ctx, saveResult);
    std::string ret = tempStr;
    JS_FreeCString(ctx, tempStr);
    JS_FreeValue(ctx, saveResult);
    return ret;
  }

  bool Eval(const char *code) const
  {
    JSValue result = JS_EvalThis(ctx, global, code, strlen(code), "eval", 0);
    const char *resultStr = JS_ToCString(ctx, result);
    if (resultStr) {
      SLT_Print(resultStr);
    }
    else {
      SLT_Print("<no result>");
    }
    JS_FreeCString(ctx, resultStr);

    if (JS_IsException(result)) {
      JS_FreeValue(ctx, result);
      return false;
    }

    JS_FreeValue(ctx, result);
    return true;
  }

  std::string DumpObject(JSValue val) const
  {
    const char *str;
    std::string out = "";

    str = JS_ToCString(ctx, val);
    if (str) {
      out += str;
      out += "\n";
      JS_FreeCString(ctx, str);
    }
    else {
      out += "[exception]\n";
    }

    return out;
  }

  std::string GetException() const
  {
    std::string out = "";
    JSValue exception_val = JS_GetException(ctx);
    bool is_error = JS_IsError(ctx, exception_val);
    if (is_error) {
      out += DumpObject(exception_val);
      JSValue val = JS_GetPropertyStr(ctx, exception_val, "stack");
      if (!JS_IsUndefined(val)) {
        out += DumpObject(val);
      }
      JS_FreeValue(ctx, val);
    }
    JS_FreeValue(ctx, exception_val);

    return out;
  }

  void Error(const char *message) const
  {
    SLT_Con_SetVar("engine.lastErrorStack", GetException().c_str());
    SLT_Error(ERR_GAME, message);
  }
};

bool loop = true;
SLTJSInstance *instance;
std::string saveState = "";

void main_loop()
{
  const conVar_t *errVar = SLT_Con_GetVar("engine.errorMessage");
  if (instance == nullptr && strlen(errVar->string) == 0) {
    SLT_Asset_ClearAll();
    instance = new SLTJSInstance();
    if (!instance->Init()) {
      instance->Error("Exception while parsing main.js");
      delete instance;
      instance = nullptr;
      return;
    }

    if (!instance->CallStart(saveState.c_str())) {
      delete instance;
      instance = nullptr;
      return;
    }
  }

  // bool ranUpdate = false; // FIXME: was this important? i forget
  double dt = SLT_StartFrame();
  if (dt < 0) {
    loop = false;
    SLT_EndFrame();
    return;
  }

  static bool show_memory_usage;
  if (SLT_Con_GetVar("con.active")->boolean) {
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("JavaScript")) {
        ImGui::MenuItem("Memory Usage", nullptr, &show_memory_usage);
        if (ImGui::MenuItem("Run GC")) SLT_SendConsoleCommand("js_rungc");
        if (ImGui::MenuItem("Save & Reload")) SLT_SendConsoleCommand("js_reload");
        if (ImGui::MenuItem("Full Reload")) SLT_SendConsoleCommand("js_reload nosave");

        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  }

  if (show_memory_usage && instance) {
    if (ImGui::Begin("Memory Usage", &show_memory_usage, ImGuiWindowFlags_AlwaysAutoResize)) {
      using namespace ImGui;
      JSRuntime *rt = JS_GetRuntime(instance->ctx);
      JSMemoryUsage usage;
      JS_ComputeMemoryUsage(rt, &usage);
      Text("memory used count: %lld", usage.memory_used_count);
      Text("memory used size: %lld", usage.memory_used_size);
      Separator();
      Text("malloc count: %lld", usage.malloc_count);
      Text("malloc size: %lld", usage.malloc_size);
      Text("malloc limit: %lld", usage.malloc_limit);
      Separator();
      Text("arrays: %lld", usage.array_count);
      Text("fast arrays: %lld (%lld elements)", usage.fast_array_count, usage.fast_array_elements);
      Separator();
      Text("c functions: %lld", usage.c_func_count);
      Text("functions: %lld", usage.js_func_count);
      Text("function code size: %lld bytes", usage.js_func_code_size);
      Separator();
      Text("atoms: %lld (%lld bytes)", usage.atom_count, usage.atom_size);
      Text("binary objects: %lld (%lld bytes)", usage.binary_object_count, usage.binary_object_size);
      Text("objects: %lld (%lld bytes)", usage.obj_count, usage.obj_size);
      Text("properties: %lld (%lld bytes)", usage.prop_count, usage.prop_size);
      Text("shapes: %lld (%lld bytes)", usage.shape_count, usage.shape_size);
      Text("strings: %lld (%lld bytes)", usage.str_count, usage.str_size);
    }
    ImGui::End();
  }

  if (instance) {
    Timing_Start("total");

    Timing_Start("update");
    if (!instance->CallUpdate(dt)) {
      instance->Error("Exception while calling Update()");
      delete instance;
      instance = nullptr;
    }
    double updTime = Timing_End("update");

    Timing_Start("draw");
    if (instance && !instance->CallDraw()) {
      instance->Error("Exception while calling Draw()");
      delete instance;
      instance = nullptr;
    }
    double drawTime = Timing_End("draw");

    Timing_Start("submit");
    DC_Submit();
    double submitTime = Timing_End("submit");

    double totalTime = Timing_End("total");

    if (SLT_Con_GetVar("vid.showfps")->boolean) {
      int width;
      SLT_GetResolution(&width, nullptr);
      ImGui::SetNextWindowPos(ImVec2(width - 200, 50));
      ImGui::SetNextWindowSize(ImVec2(200, 0));
      ImGui::Begin("##fps2",
                   nullptr,
                   ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoInputs);
      ImGui::Text("upd: %0.2fms (5s: %0.2fms)", updTime, Timer_Max("update"));
      ImGui::Text("drw: %0.2fms (5s: %0.2fms)", drawTime, Timer_Max("draw"));
      ImGui::Text("sub: %0.2fms (5s: %0.2fms)", submitTime, Timer_Max("submit"));
      ImGui::Text("sum: %0.2fms (5s: %0.2fms)", totalTime, Timer_Max("total"));

      ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
      ImGui::End();
    }
  }
  else {
    DC_Clear(0, 0, 0, 255);
    DC_Submit();
  }

  SLT_EndFrame();
  SLT_UpdateLastFrameTime();
}

int main(int argc, char *argv[])
{
  SLT_Init(argc, argv);
  SLT_Con_SetDefaultCommandHandler([]() {
    const char *cmd = SLT_Con_GetArgs(0);
    if (instance) {
      instance->Eval(cmd);
      return true;
    }
    return false;
  });

  SLT_Con_AddCommand("js_reload", []() {
    SLT_Con_SetVar("engine.errorMessage", "");
    SLT_Con_SetVar("engine.lastErrorStack", "");

    if (SLT_Con_GetArgCount() > 1) {
      saveState = "";
    }
    else if (instance) {
      saveState = instance->CallSave();
    }

    if (instance) {
      delete instance;
      instance = nullptr;
    }
  });

  SLT_Con_AddCommand("js_eval", []() {
    if (instance) {
      const char *js = SLT_Con_GetArgs(1);
      instance->Eval(js);
    }
  });

  SLT_Con_AddCommand("js_debug", []() {
    if (instance) {
      const char *address = SLT_Con_GetArgs(1);
      if (address != NULL) js_debugger_wait_connection(instance->ctx, address);
    }
  });

  SLT_Con_AddCommand("js_rungc", []() {
    if (instance) {
      JS_RunGC(instance->rt);
    }
  });

  ImGui::SetCurrentContext((ImGuiContext *)SLT_GetImguiContext());

#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(main_loop, 0, 1);
#else
  while (loop) {
    main_loop();
  }

  if (instance) {
    delete instance;
  }

  SLT_Shutdown();
#endif
}