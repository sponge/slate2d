#pragma once
// types that are shared across the dll and engine
#include <stdint.h>

#ifdef _MSC_VER 
#define STRFUNCS
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

typedef unsigned char byte;
typedef void(*xcommand_t) (void);

typedef struct {
	byte	*data;
	int		maxsize;
	int		cursize;
} cmd_t;

// paramters for command buffer stuffing
typedef enum {
	EXEC_NOW,			// don't return until completed
	EXEC_INSERT,		// insert at current position, but don't run yet
	EXEC_APPEND			// add to end of the command buffer (normal case)
} cbufExec_t;

// parameters to the main Error routine
typedef enum {
	ERR_NONE,
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
} errorParm_t;

// CVARS

#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define	CVAR_SYSTEMINFO		2	// these cvars will be duplicated on all clients
#define	CVAR_INIT			4	// don't allow change from console at all,
// but can be set from the command line
#define	CVAR_LATCH			8	// will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet
#define	CVAR_ROM			16	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	32	// created by a set command
#define	CVAR_TEMP			64	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			128	// can not be changed if cheats are disabled
#define CVAR_NORESTART		256	// do not clear when a cvar_restart is issued

#define	MAX_CVAR_VALUE_STRING	256

// nothing outside the Cvar_*() functions should modify these fields!
typedef struct cvar_s {
	char		*name;
	char		*string;
	char		*resetString;		// cvar_restart will reset to this value
	char		*latchedString;		// for CVAR_LATCH vars
	int			flags;
	bool	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	float		value;				// atof( string )
	int			integer;			// atoi( string )
} cvar_t;

typedef struct {
	cvar_t	   		**cvar;
	const char		*cvarName;
	const char		*defaultString;
	int	     		cvarFlags;
} cvarTable_t;

typedef struct {
	int			down[2];		// key nums holding it down
	int64_t		downtime;		// musec timestamp
	int64_t		musec;			// musec down this frame if both a down and up happened
	bool		active;			// current state
	bool		wasPressed;		// set when down, not cleared when up
} kbutton_t;

typedef struct {
	int width, height;
} ClientInfo;

// SCENE

class Scene {
public:
	virtual ~Scene() {};
	virtual void Startup(ClientInfo* i) = 0;
	virtual void Update(double dt) = 0;
	virtual void Render() = 0;
	virtual void Console(const char *line) = 0;
};

// RENDER COMMANDS

#define	MAX_RENDER_COMMANDS	0x40000

typedef struct {
	byte	cmds[MAX_RENDER_COMMANDS];
	int		used;
} renderCommandList_t;

#define COLOR_FILL 0
#define COLOR_STROKE 1
typedef struct {
	byte	commandId;
	float	color[4];
} setColorCommand_t;

typedef struct {
	byte commandId;
} resetTransformCommand_t;

typedef struct {
	byte	commandId;
	float	transform[6];
} transformCommand_t;

typedef struct {
	byte	commandId;
	float	angle;
} rotateCommand_t;

typedef struct {
	byte	commandId;
	float	x;
	float	y;
} translateCommand_t;

typedef struct {
	byte commandId;
	float x, y, w, h;
} setScissorCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float	x, y, w, h;
} drawRectCommand_t;

typedef struct {
	byte	commandId;
	unsigned int fntId;
	unsigned int size;
	float lineHeight;
	int align;
} setTextStyleCommand_t;

// strSz is the size of the string. the actual string lives
// in the buffer right after this command so it can be any
// reasonable size.
typedef struct {
	byte commandId;
	float x, y, w;
	unsigned int strSz;
} drawTextCommand_t;

typedef struct {
	byte commandId;
	unsigned int fntId;
	float x, y, scale;
	unsigned int strSz;
} drawBmpTextCommand_t;

#define FLIP_H 1
#define FLIP_V 2
#define FLIP_DIAG 4

typedef struct {
	byte commandId;
	float x, y, w, h, ox, oy, angle, alpha, scale;
	byte flipBits;
	unsigned int imgId;
	unsigned int shaderId;
} drawImageCommand_t;

typedef struct {
	byte commandId;
	unsigned int spr;
	int id;
	float x, y;
	float alpha;
	float scale;
	byte flipBits;
	int w, h;
} drawSpriteCommand_t;

typedef struct {
	byte commandId;
	float x1, y1, x2, y2;
} drawLineCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float x, y, radius;
} drawCircleCommand_t;

typedef struct {
	byte	commandId;
	byte	outline;
	float x1, y1, x2, y2, x3, y3;
} drawTriCommand_t;

typedef struct {
	byte commandId;
	float x, y;
	unsigned int mapId;
	unsigned int layer, cellX, cellY, cellW, cellH;
} drawMapCommand_t;

typedef enum {
	RC_END_OF_LIST,
	RC_SET_COLOR,
	RC_SET_TEXT_STYLE,
	RC_RESET_TRANSFORM,
	RC_TRANSFORM,
	RC_ROTATE,
	RC_TRANSLATE,
	RC_SET_SCISSOR,
	RC_DRAW_RECT,
	RC_DRAW_TEXT,
	RC_DRAW_BMPTEXT,
	RC_DRAW_IMAGE,
	RC_DRAW_SPRITE,
	RC_DRAW_LINE,
	RC_DRAW_CIRCLE,
	RC_DRAW_TRI,
	RC_DRAW_MAP_LAYER,
} renderCommand_t;

// ASSETS

typedef int AssetHandle;

#define INVALID_ASSET -1

typedef enum {
	ASSET_ANY,
	ASSET_IMAGE,
	ASSET_SPRITE,
	ASSET_SPEECH,
	ASSET_SOUND,
	ASSET_MOD,
	ASSET_FONT,
	ASSET_BITMAPFONT,
	ASSET_TILEMAP,
	ASSET_MAX
} AssetType_t;

static const char* assetStrings[] = {
	"ASSET_ANY",
	"ASSET_IMAGE",
	"ASSET_SPRITE",
	"ASSET_SPEECH",
	"ASSET_SOUND",
	"ASSET_MOD",
	"ASSET_FONT",
	"ASSET_BITMAPFONT",
	"ASSET_TILEMAP",
	"ASSET_MAX"
};

typedef struct {
	AssetHandle id;
	AssetType_t type;
	bool loaded;
	char name[64];
	char path[64];
	int flags;
	void *resource;
} Asset;

typedef struct {
	unsigned int hnd;
	int w, h;
} Image;

typedef struct {
	Image *image;
	int maxId;
	int imageWidth, imageHeight;
	int spriteWidth, spriteHeight;
	int marginX, marginY;
	int rows, cols;
} Sprite;

typedef struct {
	int x, y;
} MousePosition;