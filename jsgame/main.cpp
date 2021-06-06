#include "../src/slate2d.h"
#include <imgui.h>
#include <string>
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
	const char *root = SLT_FS_RealDir("/js/main.js");

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
	JSValue main;

	SLTJSInstance() {
		rt = JS_NewRuntime();
		//JS_SetMaxStackSize(rt, 1048576);
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
		// note we don't free main since it's a global object managed by the js

		JS_FreeContext(ctx);
		JS_FreeRuntime(rt);
	}

	bool Init() {
		const char *import = "import main from './js/main.js'; \
		typeof main == 'function' ? globalThis.main = new main() : globalThis.main = main";

		JSValue imported = JS_Eval(ctx, import, strlen(import), "<import>", JS_EVAL_TYPE_MODULE);
		if (JS_IsException(imported)) {
			return false;
		}

		main = JS_GetPropertyStr(ctx, global, "main");
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
		JSValue jsResult = JS_Call(ctx, startFunc, main, 1, &jsState);
		if (JS_IsException(jsResult)) {
			return false;
		}

		JS_FreeValue(ctx, jsResult);
		return true;
	}

	bool CallUpdate(float dt) const {
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

	bool CallDraw() const {
		JSValue jsResult = JS_Call(ctx, drawFunc, main, 0, nullptr);
		if (JS_IsException(jsResult)) {
			return false;
		}

		JS_FreeValue(ctx, jsResult);
		return true;
	}

	std::string CallSave() const {
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

	bool Eval(const char *code) const {
		JSValue result = JS_EvalThis(ctx, global, code, strlen(code), "eval", 0);
		const char *resultStr = JS_ToCString(ctx, result);
		if (resultStr) {
			SLT_Print(resultStr);
		} else {
			SLT_Print("<no result>");
		}

		if (JS_IsException(result)) {
			JS_FreeValue(ctx, result);
			return false;
		}

		JS_FreeValue(ctx, result);
		return true;
	}

	std::string DumpObject(JSValue val) const {
    const char *str;
		std::string out = "";
    
    str = JS_ToCString(ctx, val);
    if (str) {
			out += str;
			out += "\n";
			JS_FreeCString(ctx, str);
    } else {
			out += "[exception]\n";
    }

		return out;
	}

	std::string GetException() const {
		std::string out = "";
    JSValue exception_val = JS_GetException(ctx);
    bool is_error = JS_IsError(ctx, exception_val);
    out += DumpObject(exception_val);
    if (is_error) {
        JSValue val = JS_GetPropertyStr(ctx, exception_val, "stack");
        if (!JS_IsUndefined(val)) {
            out += DumpObject(val);
        }
        JS_FreeValue(ctx, val);
    }
    JS_FreeValue(ctx, exception_val);

		return out;
	}

	void Error(const char *message) const {
			SLT_Con_SetVar("engine.lastErrorStack", GetException().c_str());
			SLT_Error(ERR_GAME, message);
	}
};

bool loop = true;
SLTJSInstance *instance;
std::string state = "";

void main_loop() {
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

		if (!instance->CallStart(state.c_str())) {
			instance->Error("Exception while calling Start()");
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

	if (instance) {
		if (!instance->CallUpdate(dt)) {
				instance->Error("Exception while calling Update()");
				delete instance;
				instance = nullptr;
		}

		if (!instance->CallDraw()) {
				instance->Error("Exception while calling Draw()");
				delete instance;
				instance = nullptr;
		}
	} else {
		DC_Clear(0, 0, 0, 255);
		DC_Submit();
	}

	SLT_EndFrame();
	SLT_UpdateLastFrameTime();
}

int main(int argc, char* argv[]) {
	SLT_Init(argc, argv);
	SLT_Con_SetDefaultCommandHandler([] () {
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
		if (instance) {
			state = SLT_Con_GetArgCount() > 1 ? "" : instance->CallSave();
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
			const char *js = SLT_Con_GetArgs(1);
        if (js != NULL)
            js_debugger_wait_connection(instance->ctx, js);
		}
	});

	ImGui::SetCurrentContext((ImGuiContext*)SLT_GetImguiContext());

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