#include <physfs.h>
#include "console/console.h"

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

void FS_Init(const char *argv0) {
	PHYSFS_init(argv0);
	auto fs_basepath = Cvar_Get("fs_basepath", PHYSFS_getBaseDir(), CVAR_INIT);
	auto fs_game = Cvar_Get("fs_game", "base", CVAR_INIT);

	PHYSFS_mount(va("%s/%s", fs_basepath->string, fs_game->string), "/", 1);

	const char *archiveExt = "pk3";
	char **rc = PHYSFS_enumerateFiles("/");
	char **i;
	size_t extlen = strlen(archiveExt);
	char *ext;

	for (i = rc; *i != NULL; i++) {
		size_t l = strlen(*i);
		if ((l > extlen) && ((*i)[l - extlen - 1] == '.')) {
			ext = (*i) + (l - extlen);
			if (strcasecmp(ext, archiveExt) == 0) {
				PHYSFS_mount(va("%s/%s/%s", fs_basepath->string, fs_game->string, *i), "/", 0);
			}
		}
	}

	PHYSFS_freeList(rc);

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

	int read_sz = PHYSFS_read(f, *buffer, 1, sz);

	if (read_sz == -1) {
		auto lastErr = PHYSFS_getLastError();
		Com_Printf("FS err: %s", lastErr);
	}

	PHYSFS_close(f);

	return read_sz;
}