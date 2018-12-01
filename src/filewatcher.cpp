#include <SDL/SDL.h>
#include <physfs.h>
#include "console/console.h"
#include "main.h"

#define MAX_CHECK_FILES 1024
static char sourceFiles[MAX_CHECK_FILES][MAX_QPATH];
static PHYSFS_sint64 sourceTime[MAX_CHECK_FILES];
static int filesCount = 0;
static bool fileChanged = false;
static SDL_Thread *thread;

static int FileWatcher_Thread(void *ptr) {
	NOTUSED(ptr);
	while (true) {
		for (int i = 0; i < filesCount; i++) {
			auto mtime = PHYSFS_getLastModTime(sourceFiles[i]);
			if (mtime > sourceTime[i]) {
				fileChanged = true;
				return 0;
			}
		}

		SDL_Delay(250);
	}
}

void FileWatcher_TrackFile(const char *path) {
	if (filesCount > MAX_CHECK_FILES) {
		Com_Printf("can't track anymore files!\n");
		return;
	}

	PHYSFS_Stat stat;
	int err = PHYSFS_stat(path, &stat);

	if (err == 0) {
		Com_Printf("can't stat file %s\n", path);
		return;
	}

	for (int i = 0; i < filesCount; i++) {
		if (strcmp(sourceFiles[i], path) == 0) {
			return;
		}
	}

	strncpy(sourceFiles[filesCount], path, MAX_QPATH);

	sourceTime[filesCount] = stat.modtime;
	filesCount++;
	Com_Printf("now tracking file for changes: %s\n", path);
}

void FileWatcher_TrackRecursive(const char *path) {
	int err;
	PHYSFS_Stat stat;

	char **files = PHYSFS_enumerateFiles(path);
	char **i;
	for (i = files; *i != NULL; i++) {
		const char *fullPath = CopyString(va("%s/%s", path, *i));
		err = PHYSFS_stat(fullPath, &stat);
		if (err == 0) {
			Com_Printf("can't stat file %s\n", path);
			return;
		}

		if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
			FileWatcher_TrackRecursive(fullPath);
			free((void*)fullPath);
			continue;
		}
		FileWatcher_TrackFile(fullPath);
		free((void*)fullPath);
	}
	PHYSFS_freeList(files);
}

void Cmd_TrackPath_f() {
	int c = Cmd_Argc();

	if (c < 2) {
		Com_Printf("trackfile [path] : track a directory or file for changes\n");
		return;
	}

	const char *path = Cmd_Argv(1);

	PHYSFS_Stat stat;
	int err = PHYSFS_stat(path, &stat);

	if (err == 0) {
		Com_Printf("can't stat file %s\n", path);
		return;
	}

	if (stat.filetype != PHYSFS_FILETYPE_DIRECTORY) {
		FileWatcher_TrackFile(path);
	}
	else {
		FileWatcher_TrackRecursive(path);
	}
}

void Cmd_ClearFiles_f() {
	for (int i = 0; i < MAX_CHECK_FILES; i++) {
		sourceFiles[i][0] = '\0';
		sourceTime[i] = 0;
	}

	filesCount = 0;
	Com_Printf("cleared all file modification trackers\n");
}

void FileWatcher_StartThread() {
	fileChanged = false;
	for (int i = 0; i < filesCount; i++) {
		sourceTime[i] = PHYSFS_getLastModTime(sourceFiles[i]);
	}

	thread = SDL_CreateThread(&FileWatcher_Thread, "filewatcher", nullptr);
	SDL_DetachThread(thread);
}

void FileWatcher_Init() {
	Cmd_AddCommand("filewatcher_add", &Cmd_TrackPath_f);
	Cmd_AddCommand("filewatcher_clear", &Cmd_ClearFiles_f);
	Cvar_Get("filewatcher_execute", "", 0);

	FileWatcher_StartThread();
}

void FileWatcher_Tick() {
	if (fileChanged) {
		auto v = Cvar_VariableString("filewatcher_execute");
		Cbuf_ExecuteText(EXEC_NOW, v);

		FileWatcher_StartThread();
	}
}
