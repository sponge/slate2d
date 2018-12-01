#include <physfs.h>
#include "console/console.h"

cvar_t *fs_basepath;
cvar_t *fs_basegame;
cvar_t *fs_game;

void Cmd_Dir_f() {
	const char *path = Cmd_Argc() > 1 ? Cmd_Argv(1) : "/";
	char **rc = PHYSFS_enumerateFiles(path);
	char **i;

	Com_Printf("Directory listing of %s\n", path);
	for (i = rc; *i != NULL; i++)
		Com_Printf("%s\n", *i);

	PHYSFS_freeList(rc);
}

bool FS_Exists(const char *file) {
	if (PHYSFS_exists(file)) {
		if (!PHYSFS_isDirectory(file)) {
			return true;
		}
	}
	
	return false;
}

char** FS_List(const char *path) {
	return PHYSFS_enumerateFiles(path);
}

void FS_FreeList(void * listVar) {
	PHYSFS_freeList(listVar);
}

void FS_AddPaksFromList(char **list, const char *basePath, const char *gamePath) {
	static const char *archiveExt = "pk3";
	size_t extlen = strlen(archiveExt);

	char **i;
	char *ext;

	for (i = list; *i != NULL; i++) {
		size_t l = strlen(*i);
		if ((l > extlen) && ((*i)[l - extlen - 1] == '.')) {
			ext = (*i) + (l - extlen);
			if (strcasecmp(ext, archiveExt) == 0) {
				PHYSFS_mount(va("%s/%s/%s", basePath, gamePath, *i), "/", 1);
			}
		}
	}
}

void FS_Init(const char *argv0) {
	int err = PHYSFS_init(argv0);

	if (err == 0) {
		Com_Error(ERR_FATAL, "Error in PHYSFS_init: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
		return;
	}

	const char *baseDir = PHYSFS_getBaseDir();
	fs_basepath = Cvar_Get("fs_basepath", baseDir, CVAR_INIT);
	fs_basegame = Cvar_Get("fs_basegame", "base", CVAR_INIT);
	fs_game = Cvar_Get("fs_game", DEFAULT_GAME, CVAR_INIT);

	bool modLoaded = strlen(fs_game->string) > 0;

	char **baseFiles, **gameFiles;

	// get the file listing for the basegame dir, then immediately unmount
	const char *fullBasePath = va("%s/%s", fs_basepath->string, fs_basegame->string);
	PHYSFS_mount(fullBasePath, "/", 1);
	baseFiles = PHYSFS_enumerateFiles("/");
	PHYSFS_removeFromSearchPath(fullBasePath);

	// if fs_game is set, do the same thing for the fs_game dir
	if (modLoaded) {
		const char *fullGamePath = va("%s/%s", fs_basepath->string, fs_game->string);
		PHYSFS_mount(fullGamePath, "/", 1);
		gameFiles = PHYSFS_enumerateFiles("/");
		PHYSFS_removeFromSearchPath(fullGamePath);

		// mount the mod dir first, then mount mod PK3s
		PHYSFS_mount(va("%s/%s", fs_basepath->string, fs_game->string), "/", 1);
		FS_AddPaksFromList(gameFiles, fs_basepath->string, fs_game->string);
		PHYSFS_freeList(gameFiles);
	}

	// then mount the base game dir, then the mount base game PK3s
	PHYSFS_mount(va("%s/%s", fs_basepath->string, fs_basegame->string), "/", 1);
	FS_AddPaksFromList(baseFiles, fs_basepath->string, fs_basegame->string);
	PHYSFS_freeList(baseFiles);

	// print all the files we've found in order of priority
	Com_Printf("Current filesystem search path:\n");
	for (char **foundPath = PHYSFS_getSearchPath(); *foundPath != NULL; foundPath++) {
		Com_Printf("%s\n", *foundPath);
	}
	Com_Printf("\n");

	// add command handler for dir to view virtual filesystem
	Cmd_AddCommand("dir", Cmd_Dir_f);
}

int FS_ReadFile(const char *path, void **buffer) {
	auto f = PHYSFS_openRead(path);

	if (f == nullptr) {
		return -1;
	}

	auto sz = PHYSFS_fileLength(f);

	if (buffer == nullptr) {
		return sz;
	}
	
	*buffer = malloc(sz+1);
	memset(*buffer, 0, sz + 1);

	auto read_sz = PHYSFS_read(f, *buffer, (PHYSFS_uint32)1, sz);

	if (read_sz == -1) {
		auto lastErr = PHYSFS_getLastError();
		Com_Printf("FS err: %s", lastErr);
	}

	PHYSFS_close(f);

	return (int)read_sz;
}