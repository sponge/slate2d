#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <tmx.h>
#include <stdint.h>
#include "../src/console.h"

const char * __cdecl tempstr(const char *format, ...);

// error levels, for SLT_Con_Error
#ifndef ERR_NONE
#define ERR_NONE 0
#define ERR_GAME 1
#define ERR_FATAL 2
#endif

// for ASSET_IMAGE, should the image be loaded with linear filtering as opposed to nearest
#define IMAGEFLAGS_LINEAR_FILTER 1 << 0

// for drawing images flipped
#define FLIP_H 1
#define FLIP_V 2
#define FLIP_DIAG 4

// ASSETS

typedef int AssetHandle;

// returned if a given asset is not found
#define INVALID_ASSET -1

// enum of valid assets to load in-game. ASSET_ANY and ASSET_MAX are not valid for
// creating assets.
typedef enum {
	ASSET_ANY,
	ASSET_IMAGE,
	ASSET_SPRITE,
	ASSET_SPEECH,
	ASSET_SOUND,
	ASSET_MOD,
	ASSET_FONT,
	ASSET_BITMAPFONT,
	ASSET_TMX,
	ASSET_CANVAS,
	ASSET_SHADER,
	ASSET_MAX
} AssetType_t;

typedef struct {
	unsigned int hnd;
	int w, h;
} Image;

typedef struct {
	int x, y;
} MousePosition;

#ifdef SLT_STATIC
	#define SLT_API 
#elif _MSC_VER 
	#ifdef SLT_COMPILE_DLL
		#define SLT_API __declspec(dllexport)
	#else
		#define SLT_API __declspec(dllimport)
	#endif
#else
	#ifdef SLT_COMPILE_DLL
		#define SLT_API __attribute__ ((visibility ("default")))
	#else
		#define SLT_API
	#endif
#endif

// first function that should be call. argc and argv must be C-style argv, where argv[0] is the working path
// or the path the binary is in. needed to properly parse console and setup filesystem.
SLT_API void SLT_Init(int argc, char* argv[]);

// needs to be called before shutdown, destroys window and all subsystems.
SLT_API void SLT_Shutdown();

// can optionally be setup to handle both fatal and non-fatal errors. is called from the same call stack so
// you may wish to jump out of the main loop in order to shutdown gracefully. if no handler is specified,
// a fatal error will call exit(1).
SLT_API void SLT_Con_SetErrorHandler(void(*errHandler)(int level, const char *msg));

// when the console encounters a command it doesn't understand, the last resort will be the default command
// handler specified here. your function should return a bool, if the command was handled successfully.
SLT_API void SLT_Con_SetDefaultCommandHandler(bool(*cmdHandler)());


// should be called at the start of each frame. returns a double, if less than 0, then the loop should be
// terminated, otherwise it is the time since the last frame, in seconds.
SLT_API double SLT_StartFrame();

// should be called at the end of each frame. finalizes drawing, draws debug widgets, imgui, and will sleep
// depending on vsync/maxfps until the next frame is ready.
SLT_API void SLT_EndFrame();

// should be called after a frame is ran. this exists because you may wish to render multiple frames without
// updating inputs, such as running game logic at a lower tickrate than rendering. if this is not set, you will
// may run into issues using delay and repeated keypresses in SLT_In_ButtonPressed.
SLT_API void SLT_UpdateLastFrameTime();


// sends a string containing a command, or series of commands delimited by ; to the console system. will
// immediately fire any command handlers.
SLT_API void SLT_SendConsoleCommand(const char* text);

// prints a string of text, including printf formatting, to the engine console, and depending on configuration,
// the system console.
SLT_API void SLT_Print(const char* fmt, ...);

// raises an error through the console system. see SLT_Con_SetErrorHandler to handle errors sent through this.
// ERR_GAME are usually game-level errors that are recoverable by unloading the level or returning to the main
// menu, ERR_FATAL should result in the game exiting.
SLT_API void SLT_Error(int level, const char* error, ...);

// on platforms that are running in a window, set the title.
SLT_API void SLT_SetWindowTitle(const char* title);

// gets a console variable, creating a new one with the value of var_value if it doesn't exist, otherwise return
// the existing convar.
SLT_API const conVar_t* SLT_Con_GetVarDefault(const char* var_name, const char* var_value, int flags);

// gets a console variable, returning NULL if the variable doesn't exist.
SLT_API const conVar_t* SLT_Con_GetVar(const char* name);

// sets a console variable, and returns it, creating it if it doesn't exist.
SLT_API const conVar_t* SLT_Con_SetVar(const char* var_name, const char* value);


// if running inside a console command handler, return how many arguments have been parsed.
SLT_API int SLT_Con_GetArgCount(void);

// if running inside a console command handler, get the argument at the given position.
SLT_API const char* SLT_Con_GetArg(int arg);

// if running inside a console command handler, get a temporary string containing arguments from the given position
// to the end. the pointer returned is only guaranteed to be valid until the next call to SLT_Con_GetArgs.
SLT_API const char* SLT_Con_GetArgs(int start);

// adds a command handler for the given command name to the console. conCmd_t is a void function that takes no
// parameters. get console parameters by using SLT_Con_GetArgCount and SLT_Con_GetArg.
SLT_API void SLT_Con_AddCommand(const char* name, conCmd_t cmd);


// reads a file at path from the virtual filesystem, and writes the contents to the provided buffer. returns the
// length of the file, or -1 if the file does not exist.
SLT_API int SLT_FS_ReadFile(const char* path, void** buffer);

// finds a file at a path, and returns the real dir on disk. it may be a full filesystem path, or it may
// be the zip file that contains it.
SLT_API const char* SLT_FS_RealDir(const char *path);

// returns true or false if the file exists at the given path in the virtual filesystem.
SLT_API uint8_t SLT_FS_Exists(const char* file);

// returns a list of files in the virtual file at the given path. the list returned should be freed by calling
// SLT_FS_FreeList when done.
SLT_API char** SLT_FS_List(const char* path);

// frees a list of files generated through SLT_FS_List.
SLT_API void SLT_FS_FreeList(void* listVar);


// allocates a set of buttons, which should be high-level actions dependant on your game. all button names passed
// are automatically prefixed by +, and can be bound from the console to keyboard keys or controllers. by default,
// Slate2D will setup these buttons for you, and binds them to the keyboard and all 4 gamepads:
// "up", "down", "left", "right", "a", "b", "x", "y", "l", "r", "start", "select"
SLT_API void SLT_In_AllocateButtons(const char** buttonNames, int buttonCount);

// returns the full state of a given button. the index of the key you want should match the index of the key
// registered from SLT_IN_AllocateButtons, or the default button list. the pointer returned is valid until
// SLT_In_AllocateButtons is called again, but in general there's no need to cache the output.
SLT_API const buttonState_t* SLT_In_GetButton(int buttonNum);

// returns a simple true or false if the given button has been held down. if delay and repeat are non 0, the key
// is only considered held if the user has been holding the key for that long, and if "repeat" milliseconds have
// passed since the last activation. buttonNum should correspond to the index of the key used in
// SLT_In_AllocateButtons, or the default button list, see SLT_In_AllocateButtons.
SLT_API uint8_t SLT_In_ButtonPressed(int buttonNum, unsigned int delay, int repeat);
SLT_API MousePosition SLT_In_MousePosition();


// creates or returns an existing asset handle of an asset with the given assetType, and name. use this to load
// your game assets, see AssetType_t for the supported asset types. assets must be loaded using SLT_Asset_Load and
// SLT_AssetLoadAll before being used.
SLT_API AssetHandle SLT_Asset_Create(AssetType_t assetType, const char* name, const char* path, int flags);

// finds an asset with the given name, and returns a handle. INVALID_ASSET (-1) is returned if the asset is not found.
SLT_API AssetHandle SLT_Asset_Find(const char* name);

// load an asset with the given handle. asset loading errors are fatal. Slate2D will block until the loading
// is complete, so you can split up loading to multiple frames if desired.
SLT_API void SLT_Asset_Load(AssetHandle assetHandle);

// load all currently created but not loaded assets. asset loading errors are fatal, so completion of the function
// should mean all assets are ready to use.
SLT_API void SLT_Asset_LoadAll();

// unload all active assets, freeing memory and invaliding all asset handles.
SLT_API void SLT_Asset_ClearAll();

// creates a set of assets, specified by an INI file at the given path. this is the most convenient way of loading
// sets of game data as they don't have to live in your code. you can then use SLT_Asset_Find with only knowing 
// a name in order to get the asset handles to use them. see the example INI files in the source tree for specifics
// these are the only docs you're gettin if you don't want to read the source code, buddy.
SLT_API void SLT_Asset_LoadINI(const char* path);

// if creating an ASSET_BMPFNT, setup bitmap fonts here. glyphs should be a string where each character is in same
// order as the bitmap file. glyphWidth specifies a fixed size for bitmap fonts, otherwise Slate2D looks for a fully
// transparent row. charSpacing will add or remove pixels when rendering characters, spaceWidth is the space of the 
// space chracter (0x20) and lineHeight is how much space to leave between lines of text.
SLT_API void SLT_Asset_BMPFNT_Set(AssetHandle assetHandle, const char* glyphs, int glyphWidth, int charSpacing, int spaceWidth, int lineHeight);

// returns the width of a string, rendered with an TTF or bitmap font asset handle, at the given scale.
SLT_API int SLT_Asset_TextWidth(AssetHandle assetHandle, const char* string, float scale);

// uses the current text settings to break the string "in" into lines of text no longer than width pixels wide.
// returns a pointer to the split up string that is only valid until the next call to SLT_Asset_BreakString.
SLT_API const char* SLT_Asset_BreakString(int width, const char* in);

// if creating an ASSET_SPRITE, sets the dimensions of an individual sprite, and the spacing between sprites.
SLT_API void SLT_Asset_Sprite_Set(AssetHandle assetHandle, int width, int height, int marginX, int marginY);

// if creating an ASSET_CANVAS, sets the size of the off-screen texture that can be drawn on.
SLT_API void SLT_Asset_Canvas_Set(AssetHandle assetHandle, int width, int height);

// if creating an ASSET_SHADER, loads either shader code, or the shader at the given paths if isFile is 1.
SLT_API void SLT_Asset_Shader_Set(AssetHandle id, uint8_t isFile, const char* vs, const char* fs);

// returns image metrics for a given asset handle. 
SLT_API const Image* SLT_Get_Img(AssetHandle id);

// returns a complex tmx structure.
SLT_API const tmx_map* SLT_Get_TMX(AssetHandle id);


// plays an ASSET_SPEECH, ASSET_SOUND, or ASSET_MOD at the given settings. returns a handle that can be used to
// call SLT_Snd_Stop or SLT_Snd_PauseResume with.
SLT_API unsigned int SLT_Snd_Play(AssetHandle asset, float volume, float pan, uint8_t loop);

// stops playback of the sound handle received from SLT_Snd_Play, invalidating the handle passed in. invalid handles
// are ignored, and can safely be stopped multiple times.
SLT_API void SLT_Snd_Stop(unsigned int handle);

// pauses or resumes playback of the sound handle received from SLT_Snd_Play, but not invalidating the handle.
SLT_API void SLT_Snd_PauseResume(unsigned int handle, uint8_t pause);


// stores the resolution of the window into width and height
SLT_API void SLT_GetResolution(int *width, int *height);


// returns a pointer to the dear imgui instance in order to create complex UIs using dear imgui.
SLT_API const void* SLT_GetImguiContext();


// sets the renderer to the current 0-255 rgba values.
SLT_API void DC_SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// resets all transforms that have been applied so far.
SLT_API void DC_ResetTransform();

// scales draw operations to the current drawing target by x and y mulitplication factor.
SLT_API void DC_Scale(float x, float y);

// rotates the current drawing target by angle degrees, where 360 represents a full rotation.
SLT_API void DC_Rotate(float angle);

// translates the current drawing target by x and y pixels.
SLT_API void DC_Translate(float x, float y);

// restricts all drawing to the current drawing target by a rectangle top-left corner x,y specified in pixels.
SLT_API void DC_SetScissor(int x, int y, int w, int h);

// disables the scissor on the current drawing target.
SLT_API void DC_ResetScissor();

// sets the current drawing target, passing in an asset handle to an ASSET_CANVAS.
SLT_API void DC_UseCanvas(AssetHandle canvasId);

// resets the drawing target from a canvas to the backbuffer.
SLT_API void DC_ResetCanvas();

// enables drawing with a custom shader.
SLT_API void DC_UseShader(AssetHandle shaderId);

// disables drawing with a custom shader.
SLT_API void DC_ResetShader();

// draws a rectangle at the given screen coordinates. outline or fill can be toggled with 1 or 0.
SLT_API void DC_DrawRect(float x, float y, float w, float h, uint8_t outline);

// sets an ASSET_FONT or ASSET_BITMAPFONT to be used for all text operations. size is in points for TTF, but scaling
// factor for TTF. lineHeight is a scaling multiplier, and align is a bitmask of horizontal and vertical alignment
SLT_API void DC_SetTextStyle(AssetHandle fontId, float size, float lineHeight, int align);

// draws text at the specified x and y coordinates. h sets a maximum height for multiple line text. if len is
// specified, only that many characters will be drawn, excluding color codes.
SLT_API void DC_DrawText(float x, float y, float h, const char* text, int len);

// draws an ASSET_IMAGE at the specified location. scale is a size multiplier, flipBits allows for mirroring the
// image, and ox and oy will offset the texture position by the specified amount.
SLT_API void DC_DrawImage(unsigned int imgId, float x, float y, float w, float h, float scale, uint8_t flipBits, float ox, float oy);

// draws an ASSET_SPRITE at the specified location. scale is a multiplier, flipBits allows for mirroring the sprite,
// and w and h specify how many tiles wide or tall to draw.
SLT_API void DC_DrawSprite(unsigned int spriteId, int id, float x, float y, float scale, uint8_t flipBits, int w, int h);

// draws a line at the given coordinates.
SLT_API void DC_DrawLine(float x1, float y1, float x2, float y2);

// draws a circle with a given radius at the specified coordinates. outline or fill can be toggled with 1 or 0.
SLT_API void DC_DrawCircle(float x, float y, float radius, uint8_t outline);

// draw a triangle at the specified coordinates. outline or fill can be toggled with 1 or 0.
SLT_API void DC_DrawTri(float x1, float y1, float x2, float y2, float x3, float y3, uint8_t outline);

// draws an individual layer of an ASSET_TMX at the given coordinates. subsets of tilemaps can be drawn by using
// cellX/cellY/cellW/cellH, 0 will draw the entire layer.
SLT_API void DC_DrawMapLayer(unsigned int mapId, unsigned int layer, float x, float y, unsigned int cellX, unsigned int cellY, unsigned int cellW, unsigned int cellH);

// submit all queued up drawing commands, and clear the queued commands. this will parse and call the GL commands for
// all the drawing commands.
SLT_API void DC_Submit();

// clears the screen and fills it with the specified color, 0-255.
SLT_API void DC_Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

#ifdef __cplusplus
}
#endif