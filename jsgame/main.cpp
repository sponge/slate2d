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

JSModuleDef* physfs_module_loader(JSContext* ctx, const char* module_name, void* opaque) {
	// nasty hacky code ahead.
	// use main.js because it will return pak00.pk3 when running out of a pk3 file. this probably breaks cases
	// where you have js files in multiple pk3s though but i don't care about that right now. maybe we just need
	// to chop off (file).pk3 if it is in the end of the path?
	const char *root = SLT_FS_RealDir("/main.js");

	// if module_name resolves to the full path, chop off the prefix to give us the virtual path
	// relative to where main.js is since physicsfs doesn't support ./ or ../ at all
	const char *base = strstr(module_name, root);
	const char *virtualPath;
	if (base != nullptr) {
		virtualPath = base + strlen(root);
	} else {
		virtualPath = module_name;
	}

	// virtualPath should now be something like /test/testmodule.js
	char* script = nullptr;
	int sz = SLT_FS_ReadFile(virtualPath, (void**)&script);

	if (sz <= 0) {
		JS_ThrowReferenceError(ctx, "could not load module filename '%s'",module_name);
		return NULL;
	}

	// compile the module here. realdir is used because if we're running from the filesystem, it'll
	// give full filesystem paths that the debugger will use.
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
	JSValue saveFunc;

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
		JS_FreeValue(ctx, saveFunc);

		JS_FreeContext(ctx);
		JS_FreeRuntime(rt);
	}

	bool Init() {
		const char *import = "import main from './main.js'; globalThis.main = main";
		JSValue imported = JS_Eval(ctx, import, strlen(import), "<import>", JS_EVAL_TYPE_MODULE);
		if (JS_IsException(imported)) {
			js_std_dump_error(ctx);
			return false;
		}

		JSValue main = JS_GetPropertyStr(ctx, global, "main");
		if (!JS_IsObject(main)) {
			SLT_Error(ERR_GAME, "main.js did not export a module.");
			return false;
		}

		updateFunc = JS_GetPropertyStr(ctx, main, "update");
		drawFunc = JS_GetPropertyStr(ctx, main, "draw");
		startFunc = JS_GetPropertyStr(ctx, main, "start");
		saveFunc = JS_GetPropertyStr(ctx, main, "save");

		JS_FreeValue(ctx, main);

		if (!JS_IsFunction(ctx, updateFunc)) {
			SLT_Error(ERR_GAME, "Module did not export an update function.");
			return false;		
		}

		if (!JS_IsFunction(ctx, drawFunc)) {
			SLT_Error(ERR_GAME, "Module did not export a draw function.");
			return false;		
		}

		if (!JS_IsFunction(ctx, startFunc)) {
			SLT_Error(ERR_GAME, "Module did not export a start function.");
			return false;		
		}

		return true;
	}

	bool CallStart(const char *state) const {
		JSValueConst jsState = state == nullptr || strlen(state) == 0 ? JS_UNDEFINED : JS_NewString(ctx, state);
		JSValue jsResult = JS_Call(ctx, startFunc, global, 1, &jsState);
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

	std::string CallSave() const {
		if (!JS_IsFunction(ctx, saveFunc)) {
			return "";
		}

		JSValue saveResult = JS_Call(ctx, saveFunc, global, 0, nullptr);
		if (!JS_IsString(saveResult)) {
			JS_FreeValue(ctx, saveResult);
			return "";
		}

		const char *tempStr = JS_ToCString(ctx, saveResult);
		std::string ret = tempStr;
		JS_FreeCString(ctx, tempStr);
		return ret;
	}

	bool Eval(const char *code) const {
		JSValue result = JS_EvalThis(ctx, global, code, strlen(code), "eval", 0);
		const char *resultStr = JS_ToCString(ctx, result);
		Con_Print(resultStr);

		if (JS_IsException(result)) {
			JS_FreeValue(ctx, result);
			return false;
		}

		JS_FreeValue(ctx, result);
		return true;
	}
};

bool loop = true;
SLTJSInstance *instance;
std::string state = "";

void main_loop() {
	conVar_t *errVar = Con_GetVar("engine.errorMessage");
	if (instance == nullptr && strlen(errVar->string) == 0) {
		SLT_Asset_ClearAll();
		instance = new SLTJSInstance();
		if (!instance->Init()) {
			delete instance;
			instance = nullptr;
			SLT_Error(ERR_GAME, "JS error while loading main.js.");
			return;
		}

		if (!instance->CallStart(state.c_str())) {
			delete instance;
			instance = nullptr;
			SLT_Error(ERR_GAME, "JS error while calling start function.");
			return;
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

	Con_AddCommand("js_reload", []() {
		state = instance->CallSave();
		delete instance;
		instance = nullptr;
	});

	Con_AddCommand("js_clearState", []() {
		state = "";
	});

	Con_AddCommand("js_eval", []() {
		const char *js = SLT_Con_GetArgs(1);
		instance->Eval(js);
	});

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