#include "console.h"

#include <string.h>
#include <stdio.h>
#include "../external/sds.h"


// Built in commands

void Cmd_Echo_f() {
	sds text = sdsnew(Con_GetRawArgs());
	text = sdstrim(text, " ");
	text = sdscat(text, "\n");

	Con_Print(text);
	sdsfree(text);
}

void Cmd_Set_f() {
	// set a cvar
}

void Cmd_ListCvars_f() {
	// list a cvar
}

void Cmd_Vstr_f() {
	// run the contents of argv[1] as a command
	// i be worrying about stack overflows? ex lots of nested vstr
}

void Cmd_Exec_f() {
	// if it doesn't end in cfg, add it
}

void Cmd_ListCmds_f() {
	map_iter_t iter = map_iter(&con->cmds);
	int count = 0;
	int filteredCount = 0;
	const char *key = NULL;
	const char *search = Con_GetArgsCount() > 1 ? Con_GetArg(1) : NULL;

	while ((key = map_next(&con->cmds, &iter))) {
		count++;

		if (search != NULL && strstr(key, search) == NULL) {
			continue;
		}

		Con_Printf("%s\n", key);
		filteredCount++;
	}

	if (search != NULL) {
		Con_Printf("found %i commands (total %i)\n", filteredCount, count);
	}
	else {
		Con_Printf("total %i commands\n", count);
	}
}

// Main Console

void Con_Init(conState_t *newCon) {
    map_init(&newCon->cvars);
	map_init(&newCon->cmds);

	Con_SetActive(newCon);

	Con_AddCommand("echo", Cmd_Echo_f);
	Con_AddCommand("listcmds", Cmd_ListCmds_f);
	Con_AddCommand("listcvars", Cmd_ListCvars_f);
	Con_AddCommand("set", Cmd_Set_f);
	Con_AddCommand("vstr", Cmd_Vstr_f);
	Con_AddCommand("exec", Cmd_Vstr_f);
}

void Con_SetActive(conState_t *newCon) {
	con = newCon;
}

void Con_Print(const char *text) {
	if (con->handlers.print) {
		con->handlers.print(text);
	}
	else {
		printf(text);
	}
}

void Con_Printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	sds text = sdscatvprintf(sdsempty(), fmt, args);
	va_end(args);

	Con_Print(text);

	sdsfree(text);
}

void Con_Shutdown() {
	map_deinit(&con->cvars);
	map_deinit(&con->cmds);
}

// Command handling
void Con_AddCommand(const char *name, conCmd_t cb) {
	map_set(&con->cmds, name, cb);
}

void Con_RemoveCommand(const char *name) {
	map_remove(&con->cmds, name);
}

void Con_Execute(const char *cmd) {
	con->cmd = cmd;
	con->argv = sdssplitargs(cmd, &con->argc);

	conCmd_t *handler = map_get(&con->cmds, con->argv[0]);

	if (handler) {
		(*handler)();
	}
	else if (/* its a cvar */ false) {
		// handle by printing or setting the value
	}
	else if (con->handlers.unhandledCommand) {
		// handled through callback
		con->handlers.unhandledCommand();
	}
	else {
		Con_Printf("unknown command: %s\n", con->argv[0]);
	}

	sdsfreesplitres(con->argv, con->argc);
	sdsfree(con->tempArgs);
	con->cmd = NULL;
	con->argv = NULL;
	con->argc = 0;
}

const char *Con_GetArg(unsigned int i) {
	return i > con->argc ? "" : con->argv[i];
}

int Con_GetArgsCount() {
	return con->argc;
}

const char *Con_GetArgs(unsigned int start) {
	if (start > con->argc) {
		return "";
	}

	con->tempArgs = sdsempty();

	for (start; start < con->argc; start++) {
		con->tempArgs = sdscat(con->tempArgs, con->argv[start]);
	}

	return con->tempArgs;
}

const char *Con_GetRawArgs() {
	size_t cmdLen = sdslen(con->argv[0]);

	return &con->cmd[cmdLen + 1];
}