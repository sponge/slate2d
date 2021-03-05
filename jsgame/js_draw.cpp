#include "../src/slate2d.h"
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

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