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

// TODO: eval command, savestate/loadstate JS functions to call on hot reload

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

class SLTJSInstance {
public:
	JSContext *ctx;
	JSRuntime *rt;
	JSValue global;
	JSValue updateFunc;
	JSValue drawFunc;
	JSValue startFunc;

	SLTJSInstance() {
		rt = JS_NewRuntime();
		ctx = JS_NewContext(rt);
		global = JS_GetGlobalObject(ctx);

		// get console functions from here for now
		js_std_add_helpers(ctx, 0, nullptr);
		js_init_module_draw(ctx, "draw");
		js_init_module_slt(ctx, "slate2d");
		js_init_module_assets(ctx, "assets");

		JS_SetModuleLoaderFunc(rt, nullptr, physfs_module_loader, nullptr);
	}

	~SLTJSInstance() {
		JS_FreeValue(ctx, global);
		JS_FreeValue(ctx, startFunc);
		JS_FreeValue(ctx, updateFunc);
		JS_FreeValue(ctx, drawFunc);

		JS_FreeContext(ctx);
		JS_FreeRuntime(rt);
	}

	bool Init() {
		char* script = nullptr;
		int sz = SLT_FS_ReadFile("main.js", (void**)&script);

		if (sz <= 0) {
			SLT_Error(ERR_GAME, "Couldn't find main.js");
			return false;
		}

		const char *realdir = SLT_FS_RealDir("main.js");
		std::string fullpath = std::string(realdir) + "/main.js";
		JSValue val = JS_Eval(ctx, script, strlen(script), fullpath.c_str(), JS_EVAL_TYPE_MODULE);
		if (JS_IsException(val)) {
			js_std_dump_error(ctx);
			return false;
		}
		JS_FreeValue(ctx, val);

		updateFunc = JS_GetPropertyStr(ctx, global, "update");
		drawFunc = JS_GetPropertyStr(ctx, global, "draw");

		if (!JS_IsFunction(ctx, updateFunc)) {
			SLT_Error(ERR_GAME, "globalThis.update was not a function");
			return false;		
		}

		if (!JS_IsFunction(ctx, drawFunc)) {
			SLT_Error(ERR_GAME, "globalThis.draw was not a function");
			return false;		
		}

		startFunc = JS_GetPropertyStr(ctx, global, "start");
		if (!JS_IsFunction(ctx, startFunc)) {
			SLT_Error(ERR_GAME, "globalThis.start was not a function");
			return false;		
		}

		return true;
	}

	bool CallStart() const {
		JSValue jsResult = JS_Call(ctx, startFunc, global, 0, nullptr);
		if (JS_IsException(jsResult)) {
			js_std_dump_error(ctx);
			JS_FreeValue(ctx, jsResult);
			SLT_Error(ERR_GAME, "Exception in globalThis.start. Check stdout for details until I fix this.");

			return false;
		}

		JS_FreeValue(ctx, jsResult);
		return true;
	}

	bool CallUpdate(float dt) const {
		JSValue jsResult;

		JSValueConst arg = JS_NewFloat64(ctx, dt);
		jsResult = JS_Call(ctx, updateFunc, global, 1, &arg);
		if (JS_IsException(jsResult)) {
			js_std_dump_error(ctx);
			JS_FreeValue(ctx, arg);
			JS_FreeValue(ctx, jsResult);

			SLT_Error(ERR_GAME, "Exception in globalThis.update. Check stdout for details until I fix this.");
			return false;
		}

		JS_FreeValue(ctx, arg);
		JS_FreeValue(ctx, jsResult);

		return true;
	}

	bool CallDraw() const {
		JSValue jsResult = JS_Call(ctx, drawFunc, global, 0, nullptr);
		if (JS_IsException(jsResult)) {
			js_std_dump_error(ctx);
			JS_FreeValue(ctx, jsResult);

			SLT_Error(ERR_GAME, "Exception in globalThis.draw. Check stdout for details until I fix this.");
			return false;
		}

		JS_FreeValue(ctx, jsResult);
		return true;
	}
};

bool loop = true;
SLTJSInstance *instance;

void main_loop() {
	conVar_t *errVar = Con_GetVar("engine.errorMessage");
	if (instance == nullptr && strlen(errVar->string) == 0) {
		SLT_Asset_ClearAll();
		instance = new SLTJSInstance();
		if (!instance->Init()) {
			delete instance;
			instance = nullptr;
		}

		if (!instance->CallStart()) {
			delete instance;
			instance = nullptr;
		}
	}

	bool ranUpdate = false;
	double dt = SLT_StartFrame();
	if (dt < 0) {
		loop = false;
		SLT_EndFrame();
		return;
	}

	if (!instance) {
		DC_Clear(0, 0, 0, 255);
		DC_Submit();
		goto finish;
	}

	if (!instance->CallUpdate(dt)) {
		goto error;
	}

	if (!instance->CallDraw()) {
		goto error;
	}

	goto finish;

error:
	delete instance;
	instance = nullptr;

finish:
	SLT_EndFrame();
	SLT_UpdateLastFrameTime();
}

int main(int argc, char* argv[]) {
	SLT_Init(argc, argv);

	Con_AddCommand("js_reload", []() { delete instance; instance = nullptr; });

	ImGui::SetCurrentContext((ImGuiContext*)SLT_GetImguiContext());

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (loop) {
		main_loop();
	}

	delete instance;
	SLT_Shutdown();
#endif
}