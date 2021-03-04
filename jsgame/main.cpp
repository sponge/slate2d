#include "../src/slate2d.h"
#include <imgui.h>
#include <string>
#include "main.h"
extern "C" {
#include <quickjs.h>
#include <quickjs-libc.h>
#include <quickjs-debugger.h>
#include <cutils.h>
}

bool loop = true;
JSContext *ctx;
JSRuntime *rt;
JSValue global;
JSValue updateFunc;
JSValue drawFunc;

static JSValue js_draw_setcolor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int r;
    int g;
    int b;
    int a;

    if (JS_ToInt32(ctx, &r, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &g, argv[1])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &b, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &a, argv[3])) return JS_EXCEPTION;

    DC_SetColor(r,g,b,a);
    return JS_UNDEFINED;
}

static JSValue js_draw_resettransform(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    DC_ResetTransform();
    return JS_UNDEFINED;
}

static JSValue js_draw_scale(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x;
    double y;

    if (JS_ToFloat64(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[1])) return JS_EXCEPTION;

    DC_Scale(x,y);
    return JS_UNDEFINED;
}

static JSValue js_draw_rotate(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double angle;

    if (JS_ToFloat64(ctx, &angle, argv[0])) return JS_EXCEPTION;

    DC_Rotate(angle);
    return JS_UNDEFINED;
}

static JSValue js_draw_translate(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x;
    double y;

    if (JS_ToFloat64(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[1])) return JS_EXCEPTION;

    DC_Translate(x,y);
    return JS_UNDEFINED;
}

static JSValue js_draw_setscissor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int x;
    int y;
    int w;
    int h;

    if (JS_ToInt32(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &y, argv[1])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &w, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &h, argv[3])) return JS_EXCEPTION;

    DC_SetScissor(x,y,w,h);
    return JS_UNDEFINED;
}

static JSValue js_draw_resetscissor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    DC_ResetScissor();
    return JS_UNDEFINED;
}

static JSValue js_draw_usecanvas(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int canvasId;

    if (JS_ToInt32(ctx, &canvasId, argv[0])) return JS_EXCEPTION;

    DC_UseCanvas(canvasId);
    return JS_UNDEFINED;
}

static JSValue js_draw_resetcanvas(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    DC_ResetCanvas();
    return JS_UNDEFINED;
}

static JSValue js_draw_useshader(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int shaderId;

    if (JS_ToInt32(ctx, &shaderId, argv[0])) return JS_EXCEPTION;

    DC_UseShader(shaderId);
    return JS_UNDEFINED;
}

static JSValue js_draw_resetshader(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    DC_ResetShader();
    return JS_UNDEFINED;
}

static JSValue js_draw_rect(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x;
    double y;
    double w;
    double h;
    int outline;

    if (JS_ToFloat64(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &w, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &h, argv[3])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &outline, argv[4])) return JS_EXCEPTION;

    DC_DrawRect(x,y,w,h,outline);
    return JS_UNDEFINED;
}

static JSValue js_draw_settextstyle(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int fontId;
    double size;
    double lineHeight;
    int align;

    if (JS_ToInt32(ctx, &fontId, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &size, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &lineHeight, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &align, argv[3])) return JS_EXCEPTION;

    DC_SetTextStyle(fontId,size,lineHeight,align);
    return JS_UNDEFINED;
}

static JSValue js_draw_text(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x;
    double y;
    double h;
    const char * text;
    int len;

    if (JS_ToFloat64(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &h, argv[2])) return JS_EXCEPTION;
    if ((text = JS_ToCString(ctx, argv[3])) == NULL) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &len, argv[4])) return JS_EXCEPTION;

    DC_DrawText(x,y,h,text,len);
    return JS_UNDEFINED;
}

static JSValue js_draw_image(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int imgId;
    double x;
    double y;
    double w;
    double h;
    double scale;
    int flipBits;
    double ox;
    double oy;

    if (JS_ToInt32(ctx, &imgId, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &w, argv[3])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &h, argv[4])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &scale, argv[5])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &flipBits, argv[6])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &ox, argv[7])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &oy, argv[8])) return JS_EXCEPTION;

    DC_DrawImage(imgId,x,y,w,h,scale,flipBits,ox,oy);
    return JS_UNDEFINED;
}

static JSValue js_draw_sprite(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int spriteId;
    int id;
    double x;
    double y;
    double scale;
    double flipBits;
    int w;
    int h;

    if (JS_ToInt32(ctx, &spriteId, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &id, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[3])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &scale, argv[4])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &flipBits, argv[5])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &w, argv[6])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &h, argv[7])) return JS_EXCEPTION;

    DC_DrawSprite(spriteId,id,x,y,scale,flipBits,w,h);
    return JS_UNDEFINED;
}

static JSValue js_draw_line(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x1;
    double y1;
    double x2;
    double y2;

    if (JS_ToFloat64(ctx, &x1, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y1, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x2, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y2, argv[3])) return JS_EXCEPTION;

    DC_DrawLine(x1,y1,x2,y2);
    return JS_UNDEFINED;
}

static JSValue js_draw_circle(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x;
    double y;
    double radius;
    int outline;

    if (JS_ToFloat64(ctx, &x, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &radius, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &outline, argv[3])) return JS_EXCEPTION;

    DC_DrawCircle(x,y,radius,outline);
    return JS_UNDEFINED;
}

static JSValue js_draw_tri(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    double x1;
    double y1;
    double x2;
    double y2;
    double x3;
    double y3;
    int outline;

    if (JS_ToFloat64(ctx, &x1, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y1, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x2, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y2, argv[3])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x3, argv[4])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y3, argv[5])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &outline, argv[6])) return JS_EXCEPTION;

    DC_DrawTri(x1,y1,x2,y2,x3,y3,outline);
    return JS_UNDEFINED;
}

static JSValue js_draw_maplayer(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int mapId;
    int layer;
    double x;
    double y;
    int cellX;
    int cellY;
    int cellW;
    int cellH;

    if (JS_ToInt32(ctx, &mapId, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &layer, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &x, argv[2])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &y, argv[3])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &cellX, argv[4])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &cellY, argv[5])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &cellW, argv[6])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &cellH, argv[7])) return JS_EXCEPTION;

    DC_DrawMapLayer(mapId,layer,x,y,cellX,cellY,cellW,cellH);
    return JS_UNDEFINED;
}

static JSValue js_draw_submit(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    DC_Submit();
    return JS_UNDEFINED;
}

static JSValue js_draw_clear(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int r;
    int g;
    int b;
    int a;

    if (JS_ToInt32(ctx, &r, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &g, argv[1])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &b, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &a, argv[3])) return JS_EXCEPTION;

    DC_Clear(r,g,b,a);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_draw_funcs[] = {
	JS_CFUNC_DEF("setColor", 4, js_draw_setcolor),
	JS_CFUNC_DEF("resetTransform", 0, js_draw_resettransform),
	JS_CFUNC_DEF("scale", 2, js_draw_scale),
	JS_CFUNC_DEF("rotate", 1, js_draw_rotate),
	JS_CFUNC_DEF("translate", 2, js_draw_translate),
	JS_CFUNC_DEF("setScissor", 4, js_draw_setscissor),
	JS_CFUNC_DEF("resetScissor", 0, js_draw_resetscissor),
	JS_CFUNC_DEF("useCanvas", 1, js_draw_usecanvas),
	JS_CFUNC_DEF("useShader", 1, js_draw_useshader),
	JS_CFUNC_DEF("rect", 5, js_draw_rect),
	JS_CFUNC_DEF("setTextStyle", 4, js_draw_settextstyle),
	JS_CFUNC_DEF("text", 5, js_draw_text),
	JS_CFUNC_DEF("image", 9, js_draw_image),
	JS_CFUNC_DEF("line", 4, js_draw_line),
	JS_CFUNC_DEF("circle", 4, js_draw_circle),
	JS_CFUNC_DEF("tri", 7, js_draw_tri),
	JS_CFUNC_DEF("mapLayer", 7, js_draw_maplayer),
	JS_CFUNC_DEF("sprite", 8, js_draw_sprite),
	JS_CFUNC_DEF("submit", 0, js_draw_submit),
	JS_CFUNC_DEF("clear", 4, js_draw_clear),
};

static int js_draw_init(JSContext *ctx, JSModuleDef *m)
{    
    JS_SetModuleExportList(ctx, m, js_draw_funcs,
                           countof(js_draw_funcs));
    return 0;
}

JSModuleDef *js_init_module_draw(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_draw_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_draw_funcs, countof(js_draw_funcs));
    return m;
}

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