#include <SDL/SDL.h>
#include <physfs.h>
#include "console.h"
#include "main.h"
extern "C" {
#include "external/sds.h"
}

typedef struct {
	sds name;
	PHYSFS_sint64 lastModified;
} fileWatcherInfo_t;

typedef vec_t(fileWatcherInfo_t) fileInfo_vec_t;


static fileInfo_vec_t sourceFiles;
static bool fileChanged = false;
static SDL_Thread *thread;

static int FileWatcher_Thread(void *ptr) {
	NOTUSED(ptr);
	int i;
	fileWatcherInfo_t *file;

	while (true) {
		vec_foreach_ptr(&sourceFiles, file, i) {
			PHYSFS_Stat stat;
			int success = PHYSFS_stat(file->name, &stat);
			if (success && stat.modtime > file->lastModified) {
				fileChanged = true;
				return 0;
			}
		}

		SDL_Delay(250);
	}
}

bool FileWatcher_TrackFile(const char *path) {
	PHYSFS_Stat stat;
	int success = PHYSFS_stat(path, &stat);

	if (!success) {
		Con_Printf("can't stat file %s\n", path);
		return false;
	}

	int i;
	fileWatcherInfo_t *file;
	vec_foreach_ptr(&sourceFiles, file, i) {
		if (strcmp(file->name, path) == 0) {
			return false;
		}
	}

	fileWatcherInfo_t newFile;
	newFile.name = sdsnew(path);
	newFile.lastModified = stat.modtime;
	vec_push(&sourceFiles, newFile);

	return true;
}

int FileWatcher_TrackRecursive(const char *path) {
	
	PHYSFS_Stat stat;

	char **files = PHYSFS_enumerateFiles(path);
	char **i;
	int count = 0;
	for (i = files; *i != NULL; i++) {
		sds fullPath = sdscatfmt(sdsempty(), "%s/%s", path, *i);
		int success = PHYSFS_stat(fullPath, &stat);
		if (!success) {
			Con_Printf("can't stat file %s\n", path);
			return count;
		}

		if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY) {
			count += FileWatcher_TrackRecursive(fullPath);
			sdsfree(fullPath);
			continue;
		}
		if (FileWatcher_TrackFile(fullPath)) {
			count++;
		}
		sdsfree(fullPath);
	}
	PHYSFS_freeList(files);
	return count;
}

void Cmd_TrackPath_f() {
	int c = Con_GetArgsCount();

	if (c < 2) {
		Con_Printf("trackfile [path] : track a directory or file for changes\n");
		return;
	}

	const char *path = Con_GetArg(1);

	PHYSFS_Stat stat;
	int success = PHYSFS_stat(path, &stat);

	if (!success) {
		Con_Printf("can't stat file %s\n", path);
		return;
	}

	if (stat.filetype != PHYSFS_FILETYPE_DIRECTORY) {
		if (FileWatcher_TrackFile(path)) {
			Con_Printf("now tracking file for changes: %s\n", path);
		}
	}
	else {
		int count = FileWatcher_TrackRecursive(path);
		Con_Printf("now tracking %i files for changes: %s\n", count, path);
	}
}

void Cmd_ClearFiles_f() {
	int i;
	fileWatcherInfo_t *file;
	vec_foreach_ptr(&sourceFiles, file, i) {
		sdsfree(file->name);
	}

	vec_clear(&sourceFiles);

	Con_Printf("cleared all file modification trackers\n");
}

void FileWatcher_StartThread() {
	fileChanged = false;
	int i;
	fileWatcherInfo_t *file;
	vec_foreach_ptr(&sourceFiles, file, i) {
		PHYSFS_Stat stat;
		int success = PHYSFS_stat(file->name, &stat);
		file->lastModified = success ? stat.modtime : 0;
	}

	thread = SDL_CreateThread(&FileWatcher_Thread, "filewatcher", nullptr);
	SDL_DetachThread(thread);
}

void FileWatcher_Init() {
	Con_AddCommand("filewatcher_add", &Cmd_TrackPath_f);
	Con_AddCommand("filewatcher_clear", &Cmd_ClearFiles_f);
	Con_GetVarDefault("filewatcher.execute", "", 0);

	FileWatcher_StartThread();
}

void FileWatcher_Tick() {
	if (fileChanged) {
		auto v = Con_GetVarString("filewatcher.execute");
		Con_Execute(v);

		FileWatcher_StartThread();
	}
}
