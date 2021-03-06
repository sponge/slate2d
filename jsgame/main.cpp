#include "../src/slate2d.h"
#include <imgui.h>
#include <string>
#include "main.h"
#include "js_libs.h"
extern "C" {
#include <quickjs.h>
#include <quickjs-libc.h>
#include <quickjs-debugger.h>
}

bool loop = true;
JSContext *ctx;
JSRuntime *rt;
JSValue global;
JSValue updateFunc;
JSValue drawFunc;

JSModuleDef* physfs_module_loader(JSContext* ctx, const char* module_name, void* opaque) {
	char* script = nullptr;
	int sz = SLT_FS_ReadFile(module_name, (void**)&script);

	if (sz <= 0) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s'",module_name);
		return NULL;
	}

	/* compile the module */
	const char *realdir = SLT_FS_RealDir(module_name);
	std::string fullpath = realdir == nullptr ? module_name : std::string(realdir) + "/" + std::string(module_name);
	JSValue func_val = JS_Eval(ctx, (char*)script, sz, fullpath.c_str(), JS_EVAL_TYPE_MODULE|JS_EVAL_FLAG_COMPILE_ONLY);
	free(script);

	if (JS_IsException(func_val))
		return NULL;
	/* XXX: could propagate the exception */
	js_module_set_import_meta(ctx, func_val, true, false);
	/* the module is already referenced, so we must free it */
	JSModuleDef* m = (JSModuleDef*)JS_VALUE_GET_PTR(func_val);
	JS_FreeValue(ctx, func_val);

	return m;
}

void main_loop() {
	bool ranUpdate = false;
	double dt = SLT_StartFrame();
	if (dt < 0) {
		loop = false;
		SLT_EndFrame();
		return;
	}

	JSValue jsResult;

  JSValueConst arg = JS_NewFloat64(ctx, dt);
	jsResult = JS_Call(ctx, updateFunc, global, 1, &arg);
	if (JS_IsException(jsResult)) {
		js_std_dump_error(ctx);
	}
	JS_FreeValue(ctx, arg);
	JS_FreeValue(ctx, jsResult);

	DC_Clear(0, 0, 0, 255); // FIXME don't put here

	jsResult = JS_Call(ctx, drawFunc, global, 0, nullptr);
	if (JS_IsException(jsResult)) {
		js_std_dump_error(ctx);
	}
	JS_FreeValue(ctx, jsResult);

	ImGui::ShowDemoWindow();

	SLT_EndFrame();
	SLT_UpdateLastFrameTime();
}

int main(int argc, char* argv[]) {
	SLT_Init(argc, argv);

	ImGui::SetCurrentContext((ImGuiContext*)SLT_GetImguiContext());

	rt = JS_NewRuntime();
	ctx = JS_NewContext(rt);
	global = JS_GetGlobalObject(ctx);

	// get console functions from here for now
	js_std_add_helpers(ctx, argc, argv);
	js_init_module_draw(ctx, "draw");
	js_init_module_slt(ctx, "slate2d");
	js_init_module_assets(ctx, "assets");

	JS_SetModuleLoaderFunc(rt, nullptr, physfs_module_loader, nullptr);

	char* script = nullptr;
	int sz = SLT_FS_ReadFile("main.js", (void**)&script);

	if (sz <= 0) {
		SLT_Error(ERR_FATAL, "Couldn't find main.js");
		return 1;
	}

	const char *realdir = SLT_FS_RealDir("main.js");
	std::string fullpath = std::string(realdir) + "/main.js";
	JSValue val = JS_Eval(ctx, script, strlen(script), fullpath.c_str(), JS_EVAL_TYPE_MODULE);
	if (JS_IsException(val)) {
		js_std_dump_error(ctx);
		return 1;
	}
	JS_FreeValue(ctx, val);

	updateFunc = JS_GetPropertyStr(ctx, global, "update");
	drawFunc = JS_GetPropertyStr(ctx, global, "draw");

	if (!JS_IsFunction(ctx, updateFunc)) {
		SLT_Error(ERR_FATAL, "globalThis.update was not a function");
		return 1;		
	}

	if (!JS_IsFunction(ctx, drawFunc)) {
		SLT_Error(ERR_FATAL, "globalThis.draw was not a function");
		return 1;		
	}

	JSValue startFunc = JS_GetPropertyStr(ctx, global, "start");
	if (!JS_IsFunction(ctx, startFunc)) {
		SLT_Error(ERR_FATAL, "globalThis.start was not a function");
		return 1;		
	}

	JSValue jsResult = JS_Call(ctx, startFunc, global, 0, nullptr);
	if (JS_IsException(jsResult)) {
		js_std_dump_error(ctx);
	}
	JS_FreeValue(ctx, jsResult);

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (loop) {
		main_loop();
	}

	JS_FreeValue(ctx, global);
	JS_FreeValue(ctx, startFunc);
	JS_FreeValue(ctx, updateFunc);
	JS_FreeValue(ctx, drawFunc);

	JS_FreeContext(ctx);
	JS_FreeRuntime(rt);
	SLT_Shutdown();
#endif
}