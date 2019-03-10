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

	Con_SetVarFloat(varName, var->integer > 0.0f ? 0.0f : 1.0f);
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

void Cmd_Bind_f(void) {
	if (Con_GetArgsCount() < 2) {
		Con_Printf("bind <key> <command> - bind a console string to a key\n");
		return;
	}

	const char *keyStr = Con_GetArg(1);
	int keyNum = Con_GetKeyForString(keyStr);

	if (keyNum == -1) {
		Con_Printf("\"%s\" isn't a valid key\n", keyStr);
		return;
	}

	if (Con_GetArgsCount() == 2) {
		const char *bind = Con_GetBindForKey(keyNum);
		if (bind == NULL) {
			Con_Printf("\"%s\" is not bound\n", keyStr);
		}
		else {
			Con_Printf("\"%s\" = \"%s\"\n", keyStr, bind);
		}

		return;
	}
	else {
		Con_SetBind(keyNum, Con_GetArgs(2));
	}
}

void Cmd_Unbind_f(void) {
	if (Con_GetArgsCount() != 2) {
		Con_Printf("unbind <key> - remove command from key\n");
		return;
	}

	const char *keyStr = Con_GetArg(1);
	int keyNum = Con_GetKeyForString(keyStr);

	if (keyNum == -1) {
		Con_Printf("\"%s\" isn't a valid key\n", keyStr);
		return;
	}

	Con_RemoveBind(keyNum);
}

void Cmd_UnbindAll_f(void) {
	sds bind;
	int keyNum;
	vec_foreach(&con->binds, bind, keyNum) {
		Con_RemoveBind(keyNum);
	}
}

void Cmd_BindList_f(void) {
	sds bind;
	int keyNum;
	vec_foreach(&con->binds, bind, keyNum) {
		if (bind != NULL) {
			Con_Printf("%s: %s\n", Con_GetStringForKey(keyNum), bind);
		}
	}
}

// Main Console

void Con_Init(conState_t *newCon) {
    map_init(&newCon->vars);
	map_init(&newCon->cmds);
	vec_init(&newCon->binds);
	vec_init(&newCon->buttons);

	Con_SetActive(newCon);

	Con_AddCommand("echo", Cmd_Echo_f);
	Con_AddCommand("listcmds", Cmd_ListCmds_f);
	Con_AddCommand("listvars", Cmd_ListVars_f);
	Con_AddCommand("set", Cmd_Set_f);
	Con_AddCommand("toggle", Cmd_Toggle_f);
	Con_AddCommand("reset", Cmd_Reset_f);
	Con_AddCommand("vstr", Cmd_Vstr_f);

	Con_AddCommand("bind", Cmd_Bind_f);
	Con_AddCommand("unbind", Cmd_Unbind_f);
	Con_AddCommand("unbindall", Cmd_UnbindAll_f);
	Con_AddCommand("bindlist", Cmd_BindList_f);
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

	if (strcmp(var->string, value) == 0) {
		return var;
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

void Con_SetupCommandLine(int argc, char *argv[]) {
	con->sargc = argc;
	con->sargv = argv;
}

void Con_ExecuteCommandLine() {
	sds current = sdsempty();
	for (int i = 1; i < con->sargc; i++) {
		if (con->sargv[i][0] == '+') {
			Con_Execute(current);
			sdsclear(current);
			current = sdscatprintf(current, "%s ", &con->sargv[i][1]);
		}
		else {
			current = sdscatprintf(current, "%s ", con->sargv[i]);
		}

	}

	Con_Execute(current);
	sdsfree(current);

	con->sargc = 0;
	con->sargv = NULL;
}

void Con_SetVarFromStartup(const char * name) {
	if (con->sargv == NULL) {
		return;
	}

	sds sname = sdsnew(name);
	sdstolower(sname);

	sds currarg = sdsempty();

	for (int i = 1; i < con->sargc; i++) {
		if (strcmp(con->sargv[i], "+set") == 0) {
			if (i + 2 < con->sargc) {
				sdsclear(currarg);
				currarg = sdscat(currarg, con->sargv[i + 1]);
				sdstolower(currarg);
				if (sdscmp(currarg, sname) == 0) {
					Con_GetVarDefault(name, con->sargv[i + 2], CONVAR_USER);
					break;
				}
			}
		}
	}

	sdsfree(sname);
	sdsfree(currarg);
}

// input handling

void Con_AllocateKeys(int count) {
	vec_reserve(&con->binds, count);
	for (int i = 0; i < count; i++) {
		vec_push(&con->binds, NULL);
	}
}

const char *Con_GetBindForKey(int key) {
	if (key < 0 || key > con->binds.length) {
		return "";
	}

	return con->binds.data[key] == NULL ? "" : con->binds.data[key];
}

void Con_HandleKeyPress(int key, bool down, int64_t time) {
	// look up the binding, process it if its a button press
	if (key < 0 || key > con->binds.capacity) {
		Con_Printf("Con_HandleKeyPress: key %i out of range for vec length %i", key, con->binds.length);
		return;
	}

	const char *action = con->binds.data[key];

	if (action == NULL) {
		return;
	}
	else if (action[0] == '+') {
		int buttonNum;
		buttonState_t *button;

		// go through each button to see if the name of the button matches the bind
		vec_foreach_ptr(&con->buttons, button, buttonNum) {
			if (strcmp(button->name, &action[1]) == 0) {
				if (down) {
					// put this key's id in the keysheld id so we can track separate key presses
					for (int i = 0; i < 8; i++) {
						if (button->keysHeld[i] == 0) {
							button->keysHeld[i] == key;
							break;
						}
					}

					// if this is the first press
					if (button->held == false) {
						button->timestamp = time;
						button->wasPressed = true;
						button->held = true;
					}
				}
				else {
					bool anyKeyHeld = false;

					// look through the keys held for the key being let go
					for (int i = 0; i < 8; i++) {
						if (button->keysHeld[i] == key) {
							button->keysHeld[i] == 0;

							// if another key is still holding this button down, dont clear the button state
							if (button->keysHeld[i] != 0) {
								anyKeyHeld = true;
							}
							break;
						}
					}

					// no keys are holding this button down still, clear the button state
					if (anyKeyHeld == false) {
						button->held = false;
						button->timestamp = false;
					}
				}

				break;
			}
		}
	}
	else if (down) {
		Con_Execute(action);
	}
}

const char * Con_GetStringForKey(int key) {
	if (con->handlers.getStringForKey == NULL) {
		Con_Error(ERR_FATAL, "Con_GetStringForKey: input system used without setting up handlers");
		return NULL;
	}

	return con->handlers.getStringForKey(key);
}


int Con_GetKeyForString(const char *key) {
	if (con->handlers.getKeyForString == NULL) {
		Con_Error(ERR_FATAL, "Con_GetKeyForString: input system used without setting up handlers");
		return -1;
	}

	return con->handlers.getKeyForString(key);
}

void Con_SetBind(int key, const char *value) {
	if (key < 0 || key > con->binds.capacity) {
		return;
	}

	if (con->binds.data[key] != NULL) {
		sdsclear(con->binds.data[key]);
	}
	else {
		con->binds.data[key] = sdsempty();
	}

	con->binds.data[key] = sdscat(con->binds.data[key], value);
}

void Con_RemoveBind(int key) {
	if (key < 0 || key > con->binds.capacity) {
		return;
	}

	if (con->binds.data[key] != NULL) {
		sdsfree(con->binds.data[key]);
		con->binds.data[key] = NULL;
	}
}

void Con_AllocateButtons(const char **buttonNames, int buttonCount) {
	if (con->buttons.length) {
		int i;
		buttonState_t button;
		vec_foreach(&con->buttons, button, i) {
			sdsfree(button.name);
		}
	}

	vec_clear(&con->buttons);

	for (int i = 0; i < buttonCount; i++) {
		buttonState_t newButton = { 0 };
		newButton.name = sdsnew(buttonNames[i]);
		vec_push(&con->buttons, newButton);
	}
}

buttonState_t *Con_GetButton(int buttonNum) {
	if (buttonNum < 0 || buttonNum > con->buttons.length) {
		return NULL;
	}

	return &con->buttons.data[buttonNum];
}