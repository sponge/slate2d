#include "../src/slate2d.h"
#include <imgui.h>
#include "main.h"
extern "C" {
#include <quickjs.h>
#include <quickjs-libc.h>
}

bool loop = true;
JSRuntime *rt;

JSModuleDef* physfs_module_loader(JSContext* ctx, const char* module_name, void* opaque) {
	JSModuleDef* m;

	char* script = nullptr;
	int sz = SLT_FS_ReadFile(module_name, (void**)&script);

	if (sz <= 0) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s'",module_name);
		return NULL;
	}

	/* compile the module */
	JSValue func_val;

	func_val = JS_Eval(ctx, (char*)script, sz, module_name, JS_EVAL_TYPE_MODULE|JS_EVAL_FLAG_COMPILE_ONLY);
	free(script);

	if (JS_IsException(func_val))
		return NULL;
	/* XXX: could propagate the exception */
	js_module_set_import_meta(ctx, func_val, true, false);
	/* the module is already referenced, so we must free it */
	m = (JSModuleDef*)JS_VALUE_GET_PTR(func_val);
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

	DC_Clear(0, 0, 0, 255);
	DC_Submit();

	ImGui::ShowDemoWindow();

	SLT_EndFrame();
	SLT_UpdateLastFrameTime();
}

int main(int argc, char* argv[]) {
	SLT_Init(argc, argv);

	ImGui::SetCurrentContext((ImGuiContext*)SLT_GetImguiContext());

	rt = JS_NewRuntime();
	auto ctx = JS_NewContext(rt);
	JSValue global = JS_GetGlobalObject(ctx);

	JS_SetModuleLoaderFunc(rt, nullptr, physfs_module_loader, nullptr);

	char* script = nullptr;
	int sz = SLT_FS_ReadFile("main.js", (void**)&script);

	if (sz <= 0) {
		SLT_Error(ERR_FATAL, "Couldn't find main.js");
		return 1;
	}

	JS_Eval(ctx, script, strlen(script), "main.js", 0);

	JSValue updateFunc = JS_GetPropertyStr(ctx, global, "update");
	JSValue drawFunc = JS_GetPropertyStr(ctx, global, "draw");

	for (int i = 0; i < 3; ++i) {
		JSValue jsResult = JS_Call(ctx, updateFunc, global, 0, nullptr);

		int32_t result;
		JS_ToInt32(ctx, &result, jsResult);
		SLT_Print("%i\n", result);
	}

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (loop) {
		main_loop();
	}

	SLT_Shutdown();
#endif
}