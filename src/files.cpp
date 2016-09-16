#include <physfs.h>
#include "console\console.h"
#include "local.h"

void Cmd_Dir_f() {
	const char *path = Cmd_Argc() > 1 ? Cmd_Argv(1) : "/";
	char **rc = PHYSFS_enumerateFiles(path);
	char **i;

	Com_Printf("Directory listing of %s\n", path);
	for (i = rc; *i != NULL; i++)
		Com_Printf("%s\n", *i);

	PHYSFS_freeList(rc);
}

void FS_Init(const char *argv0) {
	PHYSFS_init(argv0);
	auto fs_basepath = Cvar_Get("fs_basepath", "base", CVAR_INIT);

	PHYSFS_mount(fs_basepath->string, "/", 0);

	Cmd_AddCommand("dir", Cmd_Dir_f);
}

int FS_ReadFile(const char *path, void **buffer) {
	if (PHYSFS_exists(path) == false) {
		return -1;
	}

	auto f = PHYSFS_openRead(path);
	auto sz = PHYSFS_fileLength(f);

	if (buffer == nullptr) {
		return sz;
	}

	int read_sz = PHYSFS_read(f, *buffer, 1, sz);

	return read_sz;
}