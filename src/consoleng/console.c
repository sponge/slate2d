#include "console.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../external/sds.h"

conState_t *con;

// Built in commands

void Cmd_Echo_f() {
	sds text = sdsnew(Con_GetRawArgs());
	text = sdstrim(text, " ");
	text = sdscat(text, "\n");

	Con_Print(text);
	sdsfree(text);
}

void Cmd_Set_f() {
	if (Con_GetArgsCount() != 3) {
		Con_Print("set <variable> <value> - sets variable to value, creating if necessary\n");
		return;
	}

	const char *name = Con_GetArg(1);
	const char *value = Con_GetArg(2);
	Con_SetVar(name, value);
}

void Cmd_ListVars_f() {
	map_iter_t iter = map_iter(&con->vars);
	int count = 0;
	int filteredCount = 0;
	const char *key = NULL;
	const char *search = Con_GetArgsCount() > 1 ? Con_GetArg(1) : NULL;

	while ((key = map_next(&con->vars, &iter))) {
		count++;

		if (search != NULL && strstr(key, search) == NULL) {
			continue;
		}

		Con_Printf("%s\n", key);
		filteredCount++;
	}

	if (search != NULL) {
		Con_Printf("found %i vars (total %i)\n", filteredCount, count);
	}
	else {
		Con_Printf("total %i vars\n", count);
	}
}

void Cmd_Vstr_f() {
	if (Con_GetArgsCount() != 2) {
		Con_Print("vstr <name> - executes the contents of name as a console command\n");
		return;
	}

	const char *varName = Con_GetArg(1);
	conVar_t *var = Con_GetVar(varName);
	if (var == NULL) {
		Con_Printf("couldn't execute %s, var not found\n", varName);
		return;
	}

	Con_Execute(var->string);
}

void Cmd_Reset_f() {
	if (Con_GetArgsCount() != 2) {
		Con_Print("reset <name> - resets the variable to its default value\n");
		return;
	}

	Con_ResetVar(Con_GetArg(1));
}

void Cmd_Toggle_f() {
	if (Con_GetArgsCount() != 2) {
		Con_Print("toggle <name> - toggles variable value between 1 and 0\n");
		return;
	}

	const char *varName = Con_GetArg(1);
	conVar_t *var = Con_GetVar(varName);
	if (var == NULL) {
		Con_Printf("couldn't toggle %s, var not found\n", varName);
		return;
	}

	Con_SetVarFloat(varName, var->integer > 0 ? 0 : 1);
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
    map_init(&newCon->vars);
	map_init(&newCon->cmds);

	Con_SetActive(newCon);

	Con_AddCommand("echo", Cmd_Echo_f);
	Con_AddCommand("listcmds", Cmd_ListCmds_f);
	Con_AddCommand("listvars", Cmd_ListVars_f);
	Con_AddCommand("set", Cmd_Set_f);
	Con_AddCommand("toggle", Cmd_Toggle_f);
	Con_AddCommand("reset", Cmd_Reset_f);
	Con_AddCommand("vstr", Cmd_Vstr_f);
}

void Con_SetActive(conState_t *newCon) {
	con = newCon;
}

void Con_Error(int level, const char *fmt, ...) {
	if (level == ERR_NONE) {
		return;
	}

	va_list args;

	va_start(args, fmt);
	vsnprintf(con->error, 1024, fmt, args);
	va_end(args);

	if (con->handlers.error) {
		con->handlers.error(level, con->error);
	} else {
		Con_Print(con->error);
		exit(1);
	}
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
	map_deinit(&con->vars);
	map_deinit(&con->cmds);
}

// Command handling

void Con_AddCommand(const char *name, conCmd_t cb) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	map_set(&con->cmds, sname, cb);
	sdsfree(sname);
}

void Con_RemoveCommand(const char *name) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	map_remove(&con->cmds, sname);
	sdsfree(sname);
}

// Con_RunCommand WILL free the passed string in
static void Con_RunCommand(sds cmd) {
	con->cmd = cmd;
	int argc = 0;
	con->argv = sdssplitargs(con->cmd, &argc);

	if (argc == 0) {
		return;
	}

	sdstolower(con->argv[0]);
	con->argc = (unsigned int)argc;

	conCmd_t *handler = map_get(&con->cmds, con->argv[0]);
	conVar_t *var = NULL;
	bool handled = false;

	if (handler) {
		(*handler)();
		handled = true;
	}
	else if ((var = Con_GetVar(con->argv[0])) != NULL) {
		// handle by printing or setting the value
		if (con->argc == 1) {
			Con_Printf("\"%s\" is:\"%s" "\" default:\"%s" "\"\n", var->name, var->string, var->defaultValue);
		}
		else {
			Con_SetVar(con->argv[0], Con_GetArgs(1));
		}
		handled = true;
	}
	else if (con->handlers.unhandledCommand) {
		// possibly handled through callback
		handled = con->handlers.unhandledCommand();
	}
	
	if (!handled) {
		Con_Printf("unknown command: %s\n", con->argv[0]);
	}

	sdsfreesplitres(con->argv, con->argc);
	sdsfree(con->tempArgs);
	sdsfree(con->cmd);
	con->argv = NULL;
	con->argc = 0;
	con->tempArgs = NULL;
	con->cmd = NULL;
}

void Con_Execute(const char *cmd) {
	// there may be multiple commands in this string, so we need to go through and split by
	// semicolons or new lines that are outside of quoted strings.
	const char *p = cmd;
	bool inQuotes = false;
	bool inSingleQuotes = false;
	int len = 0;
	while (*p) {
		switch (*p) {
		case '\'': 
			if (!inQuotes) {
				inSingleQuotes = !inSingleQuotes;
			}
			len++;
			break;

		case '"': 
			if (!inSingleQuotes) {
				inQuotes = !inQuotes;
			}
			len++;
			break;

		case '\n':
		case ';':
			if (inQuotes || inSingleQuotes) {
				len++;
				break;
			}

			if (len > 0) {
				sds subcmd = sdscatlen(sdsempty(), p - len, len);
				subcmd = sdstrim(subcmd, "\r\n");
				Con_RunCommand(subcmd);
			}

			inSingleQuotes = false;
			inQuotes = false;
			len = 0;
			break;

		default:
			len++;
			break;
		}
		p++;
	}

	if (len > 0) {
		sds subcmd = sdscatlen(sdsempty(), p - len, len);
		subcmd = sdstrim(subcmd, "\r\n");
		Con_RunCommand(subcmd);
	}
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
		con->tempArgs = sdscatfmt(con->tempArgs,start + 1 == con->argc ? "%s" : "%s ", con->argv[start]);
	}

	return con->tempArgs;
}

const char *Con_GetRawArgs() {
	size_t cmdLen = sdslen(con->argv[0]);

	return &con->cmd[cmdLen + 1];
}

// Convar handling

conVar_t *Con_GetVar(const char *name) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	conVar_t *var = map_get(&con->vars, sname);
	sdsfree(sname);
	return var;
}

conVar_t *Con_GetVarDefault(const char *name, const char *defaultValue, int flags) {
	if (!name || !defaultValue) {
		Con_Error(ERR_FATAL, "Con_GetVarDefault: cvar name or default value is null");
		return NULL;
	}

	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		conVar_t newVar;
		newVar.name = sdsnew(name);
		sdstolower(newVar.name);
		newVar.flags = flags;
		newVar.value = strtof(defaultValue, NULL);
		newVar.defaultValue = sdsnew(defaultValue);
		newVar.string = sdsnew(defaultValue);
		newVar.integer = atoi(defaultValue);
		newVar.boolean = !!newVar.integer;

		map_set(&con->vars, newVar.name, newVar);

		// map_set is going to copy the data so make sure we get the right one by re-getting it
		var = Con_GetVar(name);
	}
	// if the config existed before we got here, properly set the default but respect the current value
	else if ((var->flags & CONVAR_USER) && !(flags & CONVAR_USER)) {
		sdsclear(var->defaultValue);
		var->defaultValue = sdscat(var->defaultValue, defaultValue);
		var->flags &= ~CONVAR_USER;
	}

	return var;
}

const char *Con_GetVarString(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? "" : var->string;
}

float Con_GetVarFloat(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? 0.0f : var->value;
}

int Con_GetVarInt(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? 0 : var->integer;
}

bool Con_GetVarBool(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? false : var->boolean;
}

conVar_t *Con_SetVar(const char *name, const char *value) {
	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		return Con_GetVarDefault(name, value, CONVAR_USER);
	}

	if (var->flags & CONVAR_ROM) {
		Con_Printf("can't set %s, is read only\n", var->name);
		return var;
	}

	return Con_SetVarForce(name, value);
}

conVar_t *Con_SetVarFloat(const char *name, float value) {
	sds str = sdsnew("");
	str = sdscatprintf(str, "%g", value);
	conVar_t *var = Con_SetVar(name, str);
	sdsfree(str);

	return var;
}

conVar_t *Con_SetVarForce(const char *name, const char *value) {
	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		return NULL;
	}

	sdsclear(var->string);
	var->string = sdscat(var->string, value);
	sdstolower(var->string);
	var->value = strtof(value, NULL);
	var->integer = atoi(value);
	var->boolean = !!var->integer;
	var->modified = true;
	var->modifiedCount++;

	return var;
}

conVar_t *Con_ResetVar(const char *name) {
	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		return NULL;
	}

	return Con_SetVar(name, var->defaultValue);
}

void Con_ParseCommandLine(const char *cmdline) {
	con->sargv = sdssplitargs(cmdline, &con->sargc);
}

void Con_ExecuteCommandLine() {
	// FIXME: set everything and run commands

	sdsfreesplitres(con->sargv, con->sargc);
	con->sargc = 0;
	con->sargv = NULL;
}

void Con_SetVarFromStartup(const char * name) {
	if (con->sargv == NULL) {
		return;
	}

	sds sname = sdsnew(name);
	sdstolower(sname);

	for (int i = 0; i < con->sargc; i++) {
		if (strcmp(con->sargv[i], "+set") == 0) {
			if (i + 2 < con->sargc) {
				sdstolower(con->sargv[i + 1]);
				if (sdscmp(con->sargv[i + 1], sname) == 0) {
					Con_GetVarDefault(name, con->sargv[i + 2], CONVAR_USER);
					break;
				}
			}
		}
	}
	sdsfree(sname);
}
