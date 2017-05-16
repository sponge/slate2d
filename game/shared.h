#pragma once
// types that are shared across the dll and engine

#include <nanovg.h> // FIXME: this shouldn't have to be here (used in ClientInfo and Img)

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
	ERR_DISCONNECT,				// don't kill server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorParm_t;

// CVARS

#define	CVAR_ARCHIVE		1	// set to cause it to be saved to vars.rc
// used for system variables, not for player
// specific configurations
#define	CVAR_USERINFO		2	// sent to server on connect or change
#define	CVAR_SERVERINFO		4	// sent in response to front end requests
#define	CVAR_SYSTEMINFO		8	// these cvars will be duplicated on all clients
#define	CVAR_INIT			16	// don't allow change from console at all,
// but can be set from the command line
#define	CVAR_LATCH			32	// will only change when C code next does
// a Cvar_Get(), so it can't be changed
// without proper initialization.  modified
// will be set, even though the value hasn't
// changed yet
#define	CVAR_ROM			64	// display only, cannot be set by user at all
#define	CVAR_USER_CREATED	128	// created by a set command
#define	CVAR_TEMP			256	// can be set even when cheats are disabled, but is not archived
#define CVAR_CHEAT			512	// can not be changed if cheats are disabled
#define CVAR_NORESTART		1024	// do not clear when a cvar_restart is issued

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
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

typedef struct {
	cvar_t	   		**cvar;
	const char		*cvarName;
	const char		*defaultString;
	int	     		cvarFlags;
} cvarTable_t;

typedef struct {
	int			down[2];		// key nums holding it down
	unsigned	downtime;		// msec timestamp
	unsigned	msec;			// msec down this frame if both a down and up happened
	bool		active;			// current state
	bool		wasPressed;		// set when down, not cleared when up
} kbutton_t;

typedef struct {
	NVGcontext *nvg;
	int width, height;
} ClientInfo;

typedef struct {
	NVGcontext *nvg;
	unsigned int index;
	unsigned int hnd;
	int w, h;
	char name[64];
	char path[64];
} Img;

class Scene {
public:
	virtual ~Scene() {};
	virtual void Startup(ClientInfo* i) = 0;
	virtual void Update(float dt) = 0;
	virtual void Render() = 0;
};

// RENDER COMMANDS

#define	MAX_RENDER_COMMANDS	0x40000

typedef struct {
	byte	cmds[MAX_RENDER_COMMANDS];
	int		used;
} renderCommandList_t;

typedef struct {
	int		commandId;
	float	color[4];
} setColorCommand_t;

typedef struct {
	int		commandId;
	float	transform[6];
} setTransformCommand_t;

typedef struct {
	int		commandId;
	float	x, y, w, h;
} drawRectCommand_t;

typedef struct {
	int commandId;
	int align;
	float x, y;
	char text[16];
} drawTextCommand_t;

typedef enum {
	RC_END_OF_LIST,
	RC_SET_COLOR,
	RC_SET_TRANSFORM,
	RC_DRAW_RECT,
	RC_DRAW_TEXT
} renderCommand_t;