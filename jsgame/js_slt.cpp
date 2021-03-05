#include "../src/slate2d.h"
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

static JSValue js_slt_printwin(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    return JS_UNDEFINED;
}

static JSValue js_slt_error(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int level;
    const char * error;

    if (JS_ToInt32(ctx, &level, argv[0])) return JS_EXCEPTION;
    if ((error = JS_ToCString(ctx, argv[1])) == NULL) return JS_EXCEPTION;

    SLT_Error(level,error);
    return JS_UNDEFINED;
}

static JSValue js_slt_console(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    const char * text;

    if ((text = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

    SLT_SendConsoleCommand(text);
    return JS_UNDEFINED;
}

static JSValue js_slt_sndplay(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int asset;
    double volume;
    double pan;
    int loop;

    if (JS_ToInt32(ctx, &asset, argv[0])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &volume, argv[1])) return JS_EXCEPTION;
    if (JS_ToFloat64(ctx, &pan, argv[2])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &loop, argv[3])) return JS_EXCEPTION;

    SLT_Snd_Play(asset,volume,pan,loop);
    return JS_UNDEFINED;
}

static JSValue js_slt_sndstop(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int handle;

    if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;

    SLT_Snd_Stop(handle);
    return JS_UNDEFINED;
}

static JSValue js_slt_sndpauseresume(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int handle;
    int paused;

    if (JS_ToInt32(ctx, &handle, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &paused, argv[1])) return JS_EXCEPTION;

    SLT_Snd_PauseResume(handle,paused);
    return JS_UNDEFINED;
}

static JSValue js_slt_registerbuttons(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    //SLT_In_AllocateButtons();
    return JS_UNDEFINED;
}

static JSValue js_slt_buttonpressed(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    int buttonNum;
    int delay;
    int repeat;

    if (JS_ToInt32(ctx, &buttonNum, argv[0])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &delay, argv[1])) return JS_EXCEPTION;
    if (JS_ToInt32(ctx, &repeat, argv[2])) return JS_EXCEPTION;

    SLT_In_ButtonPressed(buttonNum,delay,repeat);
    return JS_UNDEFINED;
}

static JSValue js_slt_setwindowtitle(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    const char * title;

    if ((title = JS_ToCString(ctx, argv[0])) == NULL) return JS_EXCEPTION;

    SLT_SetWindowTitle(title);
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_slt_funcs[] = {
        JS_CFUNC_DEF("printWin", 0, js_slt_printwin),
        JS_CFUNC_DEF("error", 2, js_slt_error),
        JS_CFUNC_DEF("console", 1, js_slt_console),
        JS_CFUNC_DEF("sndPlay", 4, js_slt_sndplay),
        JS_CFUNC_DEF("sndStop", 1, js_slt_sndstop),
        JS_CFUNC_DEF("sndPauseResume", 2, js_slt_sndpauseresume),
        JS_CFUNC_DEF("registerButtons", 0, js_slt_registerbuttons),
        JS_CFUNC_DEF("buttonPressed", 3, js_slt_buttonpressed),
        JS_CFUNC_DEF("setWindowTitle", 1, js_slt_setwindowtitle),
};

static int js_slt_init(JSContext *ctx, JSModuleDef *m)
{    
    JS_SetModuleExportList(ctx, m, js_slt_funcs,
                           countof(js_slt_funcs));
    return 0;
}

JSModuleDef *js_init_module_slt(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_slt_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_slt_funcs, countof(js_slt_funcs));
    return m;
}