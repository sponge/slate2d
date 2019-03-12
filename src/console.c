#include "console.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../external/sds.h"

conState_t *con;

// Built in commands

// command handler for echo, prints the text to the screen
void Cmd_Echo_f() {
	sds text = sdsnew(Con_GetRawArgs());
	text = sdstrim(text, " ");
	text = sdscat(text, "\n");

	Con_Print(text);
	sdsfree(text);
}

// command handler for set, to set a convar to a given value
void Cmd_Set_f() {
	if (Con_GetArgsCount() != 3) {
		Con_Print("set <variable> <value> - sets variable to value, creating if necessary\n");
		return;
	}

	const char *name = Con_GetArg(1);
	const char *value = Con_GetArg(2);
	Con_SetVar(name, value);
}

int sortStrings(const void *a, const void *b) {
	return strcmp(*(char* const*)a, *(char* const*)b);
}

// command handler for listvars, to list all variables currently registered
void Cmd_ListVars_f() {
	map_iter_t iter = map_iter(&con->vars);
	int count = 0;
	int filteredCount = 0;
	const char *key = NULL;

	// any parameter to listvars is used as a simple substring match
	const char *search = Con_GetArgsCount() > 1 ? Con_GetArg(1) : NULL;

	// alloc a list so we can sort the output
	vec_str_t sortedKeys;
	vec_init(&sortedKeys);

	while ((key = map_next(&con->vars, &iter))) {
		count++;

		// skip if filter is specified and string doesn't match
		if (search != NULL && strstr(key, search) == NULL) {
			continue;
		}

		conVar_t *el = map_get(&con->vars, key);
		vec_push(&sortedKeys, el->name);
		filteredCount++;
	}

	vec_sort(&sortedKeys, sortStrings);

	int keyNum;
	vec_foreach(&sortedKeys, key, keyNum) {
		Con_Printf("%s\n", key);
	}

	if (search != NULL) {
		Con_Printf("found %i vars (total %i)\n", filteredCount, count);
	}
	else {
		Con_Printf("total %i vars\n", count);
	}

	vec_deinit(&sortedKeys);
}

// command handler for vstr, to run the string contents of a convar as a command
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

// command handler for reset, to reset a convar to its default value
void Cmd_Reset_f() {
	if (Con_GetArgsCount() != 2) {
		Con_Print("reset <name> - resets the variable to its default value\n");
		return;
	}

	Con_ResetVar(Con_GetArg(1));
}

// command handler for toggle, toggles convar between 0 and 1 values
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

// command handler for listcmds, prints all commands to console
void Cmd_ListCmds_f() {
	map_iter_t iter = map_iter(&con->cmds);
	int count = 0;
	int filteredCount = 0;
	const char *key = NULL;

	// any parameter to listvars is used as a simple substring match
	const char *search = Con_GetArgsCount() > 1 ? Con_GetArg(1) : NULL;

	// alloc a list so we can sort the output
	vec_str_t sortedKeys;
	vec_init(&sortedKeys);

	while ((key = map_next(&con->cmds, &iter))) {
		count++;

		// skip if filter is specified and string doesn't match
		if (search != NULL && strstr(key, search) == NULL) {
			continue;
		}

		vec_push(&sortedKeys, key);
		filteredCount++;
	}

	vec_sort(&sortedKeys, sortStrings);

	int keyNum;
	vec_foreach(&sortedKeys, key, keyNum) {
		Con_Printf("%s\n", key);
	}

	if (search != NULL) {
		Con_Printf("found %i commands (total %i)\n", filteredCount, count);
	}
	else {
		Con_Printf("total %i commands\n", count);
	}

	vec_deinit(&sortedKeys);
}

// command handler for bind, to set or show a console string to run when a key is pressed
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

	// if no new bind is specified, just print the current value
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

// command handler for unbind, to remove the console script from a key
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

// command handler for unbindall, which unbinds all keys
void Cmd_UnbindAll_f(void) {
	sds bind;
	int keyNum;
	vec_foreach(&con->binds, bind, keyNum) {
		Con_RemoveBind(keyNum);
	}
}

// command handler for listbinds, prints all keys that have commands bound to them
void Cmd_ListBinds_f(void) {
	sds bind;
	int keyNum;
	vec_foreach(&con->binds, bind, keyNum) {
		if (bind != NULL) {
			Con_Printf("%s: %s\n", Con_GetStringForKey(keyNum), bind);
		}
	}
}

// Main Console

// rtakes a pointer to a previously allocated conState_t (heap or stack) and initializes
// all dynamic structures and sets up the built in commands. the pointer passed in should remain
// valid for the lifetime of the console.
void Con_Init(conState_t *newCon) {
	map_init(&newCon->vars);
	map_init(&newCon->cmds);
	vec_init(&newCon->binds);
	vec_init(&newCon->buttons);

	Con_SetActive(newCon);

	Con_AddCommand("echo", Cmd_Echo_f);

	Con_AddCommand("listvars", Cmd_ListVars_f);
	Con_AddCommand("set", Cmd_Set_f);
	Con_AddCommand("toggle", Cmd_Toggle_f);
	Con_AddCommand("reset", Cmd_Reset_f);
	Con_AddCommand("vstr", Cmd_Vstr_f);

	Con_AddCommand("listcmds", Cmd_ListCmds_f);

	Con_AddCommand("bind", Cmd_Bind_f);
	Con_AddCommand("unbind", Cmd_Unbind_f);
	Con_AddCommand("unbindall", Cmd_UnbindAll_f);
	Con_AddCommand("listbinds", Cmd_ListBinds_f);
}

// free up all memory
void Con_Shutdown() {
	const char *key;
	map_iter_t iter = map_iter(&con->vars);

	while ((key = map_next(&con->vars, &iter))) {
		conVar_t *var = map_get(&con->vars, key);
		sdsfree(var->name);
		sdsfree(var->defaultValue);
		sdsfree(var->string);
	}
	map_deinit(&con->vars);

	map_deinit(&con->cmds);

	int i;
	sds bind;
	vec_foreach(&con->binds, bind, i) {
		sdsfree(bind);
	}
	vec_deinit(&con->binds);

	i = 0;
	buttonState_t *button;
	vec_foreach_ptr(&con->buttons, button, i) {
		sdsfree(button->name);
	}
	vec_deinit(&con->buttons);
}

// swaps out the active pointer, so we don't need to constantly query for the active console.
// doing this with a global means command handlers don't need any arugments passed in since
// they can just call any Con_ function without having to be passed the conState_t
void Con_SetActive(conState_t *newCon) {
	con = newCon;
}

// returns the active pointer, in case you want to mess with the conState directly for some
// reason
conState_t *Con_GetActive() {
	return con;
}

// raise an error. if no error handler is specified, just print it and exit.
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

// small wrapper that just calls printf if a handler isn't set.
void Con_Print(const char *text) {
	if (con->handlers.print) {
		con->handlers.print(text);
	}
	else {
		printf(text);
	}
}

// same as above but with formatting strings
void Con_Printf(const char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	sds text = sdscatvprintf(sdsempty(), fmt, args);
	va_end(args);

	Con_Print(text);

	sdsfree(text);
}

// Command handling

// add a new command to the command map, case insensitive
void Con_AddCommand(const char *name, conCmd_t cb) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	map_set(&con->cmds, sname, cb);
	sdsfree(sname);
}

// removes a command from the command map, case insensitive
void Con_RemoveCommand(const char *name) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	map_remove(&con->cmds, sname);
	sdsfree(sname);
}

// Con_RunCommand WILL free the passed string in
static void Con_RunCommand(sds cmd) {
	con->cmd = cmd;

	// setup argv/argc. sdssplitargs will not split terms in quotes.
	con->argv = sdssplitargs(con->cmd, &con->argc);

	if (con->argc == 0) {
		return;
	}

	// command handlers are case insensitive
	sdstolower(con->argv[0]);

	conCmd_t *handler = map_get(&con->cmds, con->argv[0]);
	conVar_t *var = NULL;
	bool handled = false;

	// first priority is if the string is a command, run it
	if (handler) {
		(*handler)();
		handled = true;
	}
	// if there's no command handler, check if it's a valid convar
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
	// give a set console handler a final opportunity to handle the command itself
	else if (con->handlers.unhandledCommand) {
		// possibly handled through callback
		handled = con->handlers.unhandledCommand();
	}
	
	if (!handled) {
		Con_Printf("unknown command: %s\n", con->argv[0]);
	}

	// free all potentially used members
	sdsfreesplitres(con->argv, con->argc);
	sdsfree(con->tempArgs);
	sdsfree(con->cmd);
	con->argv = NULL;
	con->argc = 0;
	con->tempArgs = NULL;
	con->cmd = NULL;
}

// main entry point. takes a string, splits it up into individual cmomands (split by ; or newline) and
// runs them one at a time.
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
			// don't track single quotes that are inside double quotes
			if (!inQuotes) {
				inSingleQuotes = !inSingleQuotes;
			}
			len++;
			break;

		case '"':
			// don't track double quotes inside single quotes
			if (!inSingleQuotes) {
				inQuotes = !inQuotes;
			}
			len++;
			break;

		case '\n':
		case ';':
			// don't process command delimiters inside quotes
			if (inQuotes || inSingleQuotes) {
				len++;
				break;
			}

			// if we have processed any part of a command, trim it and run it
			if (len > 0) {
				sds subcmd = sdscatlen(sdsempty(), p - len, len);
				subcmd = sdstrim(subcmd, "\r\n");
				Con_RunCommand(subcmd);
			}

			// reset parser state
			inSingleQuotes = false;
			inQuotes = false;
			len = 0;
			break;

		// if not a special character just walk keep walking forward
		default:
			len++;
			break;
		}

		// keep going forward
		p++;
	}

	// if we've hit the end of the string, make sure to run what's last, also trimming it
	if (len > 0) {
		sds subcmd = sdscatlen(sdsempty(), p - len, len);
		subcmd = sdstrim(subcmd, "\r\n");
		Con_RunCommand(subcmd);
	}
}

// returns empty string instead of null for invalid args
const char *Con_GetArg(int i) {
	return i < 0 ? "" : i > con->argc ? "" : con->argv[i];
}

// returns amount of arguments in currently active command
int Con_GetArgsCount() {
	return con->argc;
}


// returns a temporary string from arg start to end. uses tempArgs which will be valid
// until end of command parsing, or another function that uses tempArgs
const char *Con_GetArgs(int start) {
	if (start < 0) {
		start = 0;
	}

	if (start > con->argc) {
		return "";
	}

	con->tempArgs = sdsempty();

	for (start; start < con->argc; start++) {
		con->tempArgs = sdscatfmt(con->tempArgs,start + 1 == con->argc ? "%s" : "%s ", con->argv[start]);
	}

	return con->tempArgs;
}

// returns all the arguments past the original command
const char *Con_GetRawArgs() {
	size_t cmdLen = sdslen(con->argv[0]);

	return &con->cmd[cmdLen + 1];
}

// Convar handling

// returns a convar, returning NULL if not existant.
conVar_t *Con_GetVar(const char *name) {
	sds sname = sdsnew(name);
	sdstolower(sname);
	conVar_t *var = map_get(&con->vars, sname);
	sdsfree(sname);
	return var;
}

// returns a convar, which can be newly made.
conVar_t *Con_GetVarDefault(const char *name, const char *defaultValue, int flags) {
	if (!name || !defaultValue) {
		Con_Error(ERR_FATAL, "Con_GetVarDefault: convar name or default value is null");
		return NULL;
	}

	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		// we don't have an existing convar, setup a new one on the stack, since map_set will copy it
		conVar_t newVar;
		newVar.name = sdsnew(name);
		newVar.flags = flags;
		newVar.value = strtof(defaultValue, NULL);
		newVar.defaultValue = sdsnew(defaultValue);
		newVar.string = sdsnew(defaultValue);
		newVar.integer = atoi(defaultValue);
		newVar.boolean = !!newVar.integer;

		// make a copy of the key, but now in lower case for the hash key. keep the newVar.name in the passed
		// in case for presentation reasons
		sds key = sdsnew(newVar.name);
		sdstolower(key);
		map_set(&con->vars, key, newVar);
		sdsfree(key);

		// map_set is going to copy the data so make sure we return the right one by getting it new
		var = Con_GetVar(name);
	}
	// if the config existed before we got here, properly set the default but respect the current value
	else if ((var->flags & CONVAR_USER) && !(flags & CONVAR_USER)) {
		// clear the current string and reuse it
		sdsclear(var->defaultValue);
		var->defaultValue = sdscat(var->defaultValue, defaultValue);
		var->flags &= ~CONVAR_USER;

		// if its ROM, overwrite the current value no matter what
		if (flags & CONVAR_ROM) {
			Con_SetVarForce(name, defaultValue);
		}
	}

	return var;
}

// null-safe get string value, returns empty string if convar doesn't exist
const char *Con_GetVarString(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? "" : var->string;
}

// null-safe get float value, returns 0 if convar doesn't exist
float Con_GetVarFloat(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? 0.0f : var->value;
}

// null-safe get int value, returns 0 if convar doesn't exist
int Con_GetVarInt(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? 0 : var->integer;
}

// null-safe get bool value, returns false if convar doesn't exist
bool Con_GetVarBool(const char *name) {
	conVar_t *var = Con_GetVar(name);
	return var == NULL ? false : var->boolean;
}

// sets a convar's value, creating it if necessary
conVar_t *Con_SetVar(const char *name, const char *value) {
	conVar_t *var = Con_GetVar(name);

	// if convar doesn't exist, set it as a user var, since this is where config execution comes into
	if (var == NULL) {
		return Con_GetVarDefault(name, value, CONVAR_USER);
	}

	// don't do anything if the value hasn't changed
	if (strcmp(var->string, value) == 0) {
		return var;
	}

	// CONVAR_ROMs cant be changed
	if (var->flags & CONVAR_ROM) {
		Con_Printf("can't set %s, is read only\n", var->name);
		return var;
	}


	// CONVAR_STARTUP works because the convar is set through whatever means (usually command line parsing)
	// before Con_GetVarDefault is called giving the convar the flag
	if (var->flags & CONVAR_STARTUP) {
		Con_Printf("can't set %s, can only be set on command line\n", var->name);
		return var;
	}

	return Con_SetVarForce(name, value);
}

// shortcut to set a float value directly instead of turning it into a string
conVar_t *Con_SetVarFloat(const char *name, float value) {
	sds str = sdsnew("");
	str = sdscatprintf(str, "%g", value);
	conVar_t *var = Con_SetVar(name, str);
	sdsfree(str);

	return var;
}

// most con_setvar paths end up here, but this is what will actually set the value and update the
// convar struct. if you need to force a change for whatever reason, it can also be called to bypass
// everything.
conVar_t *Con_SetVarForce(const char *name, const char *value) {
	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		return NULL;
	}

	sdsclear(var->string);
	var->string = sdscat(var->string, value);
	var->value = strtof(value, NULL);
	var->integer = atoi(value);
	var->boolean = !!var->integer;
	var->modified = true;
	var->modifiedCount++;

	return var;
}

// resets the value to default. will obey verification
conVar_t *Con_ResetVar(const char *name) {
	conVar_t *var = Con_GetVar(name);

	if (var == NULL) {
		return NULL;
	}

	return Con_SetVar(name, var->defaultValue);
}

// stores the passed in commandline so we can scan through it later and pull out convars and finally execute it all
void Con_SetupCommandLine(int argc, char *argv[]) {
	con->sargc = argc;
	con->sargv = argv;
}

// iterates through and passes a command which is a series of arguments prefixed with a +, ex "+echo hello world"
void Con_ExecuteCommandLine() {
	sds current = sdsempty();
	for (int i = 1; i < con->sargc; i++) {
		// if we have a +, execute what's there and start building up again
		if (con->sargv[i][0] == '+') {
			Con_Execute(current);
			sdsclear(current);
			current = sdscatprintf(current, "%s ", &con->sargv[i][1]);
		}
		// append until we reach the end or find a + at the start
		else {
			current = sdscatprintf(current, "%s ", con->sargv[i]);
		}

	}

	// run whatever we have left
	Con_Execute(current);

	// all done, we are never going to use this again
	sdsfree(current);
	con->sargc = 0;
	con->sargv = NULL;
}

// pull out a +set, and the following 2 arguments from the command line and create a
// CONVAR_USER convar out of it
void Con_SetVarFromStartup(const char * name) {
	if (con->sargv == NULL) {
		return;
	}

	sds sname = sdsnew(name);
	sdstolower(sname);

	sds currarg = sdsempty();

	// skip the binary name
	for (int i = 1; i < con->sargc; i++) {
		// if we found a +set, and there are 2 arguments after, it is a candidate
		if (strcmp(con->sargv[i], "+set") == 0) {
			if (i + 2 < con->sargc) {
				// case insensitive match, lowercase and compare
				sdsclear(currarg);
				currarg = sdscat(currarg, con->sargv[i + 1]);
				sdstolower(currarg);
				if (sdscmp(currarg, sname) == 0) {
					// everything is good, set the convar
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

// reserves the amount of space needed for keys, and sets all values to NULL
void Con_AllocateKeys(int count) {
	vec_reserve(&con->binds, count);
	for (int i = 0; i < count; i++) {
		vec_push(&con->binds, NULL);
	}
}

// returns string binding for key. doesn't return NULL ever, just an empty string
const char *Con_GetBindForKey(int key) {
	if (key < 0 || key > con->binds.length) {
		return "";
	}

	return con->binds.data[key] == NULL ? "" : con->binds.data[key];
}

// main input handler, all user input should go through here instead of calling other functions directly
void Con_HandleKeyPress(int key, bool down, int64_t time) {
	// look up the binding, process it if its a button press
	if (key < 0 || key > con->binds.capacity) {
		Con_Printf("Con_HandleKeyPress: key %i out of range for vec length %i", key, con->binds.length);
		return;
	}

	const char *action = con->binds.data[key];

	// if nothing is bound, there's nothing else to do
	if (action == NULL) {
		return;
	}
	// +commands are handled specially. search to see if there is a button by this game, and mark it as held
	else if (action[0] == '+') {
		int buttonNum;
		buttonState_t *button;

		// go through each button to see if the name of the button matches the bind
		vec_foreach_ptr(&con->buttons, button, buttonNum) {
			if (strcmp(button->name, &action[1]) == 0) {
				if (down) {
					// put this key's id in the keysHeld array so we can track separate key presses
					for (int i = 0; i < 8; i++) {
						if (button->keysHeld[i] == 0) {
							button->keysHeld[i] = key;
							break;
						}
					}

					// if this is the first press, mark it as held
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
							button->keysHeld[i] = 0;
						}

						// if another key is still holding this button down, dont clear the button state
						if (button->keysHeld[i] != 0) {
							anyKeyHeld = true;
						}
					}

					// no keys are holding this button down still, clear the button state
					// don't unset wasPressed because user code can clear that at whatever frequency it wants
					if (anyKeyHeld == false) {
						button->held = false;
						button->timestamp = false;
					}
				}

				break;
			}
		}
	}
	// if the command is bound, and not a + command, just run it on keydown
	else if (down) {
		Con_Execute(action);
	}
}

// mostly wrapper for handler, only really used for bindlist
const char * Con_GetStringForKey(int key) {
	if (con->handlers.getStringForKey == NULL) {
		Con_Error(ERR_FATAL, "Con_GetStringForKey: input system used without setting up handlers");
		return NULL;
	}

	return con->handlers.getStringForKey(key);
}


// mostly wrapper for handler, used to parse bind commands and convert them to key numbers
int Con_GetKeyForString(const char *key) {
	if (con->handlers.getKeyForString == NULL) {
		Con_Error(ERR_FATAL, "Con_GetKeyForString: input system used without setting up handlers");
		return -1;
	}

	return con->handlers.getKeyForString(key);
}

// takes the key id and binds it to the passed in console script, value
void Con_SetBind(int key, const char *value) {
	if (key < 0 || key > con->binds.capacity) {
		return;
	}

	// reuse the existing string if there's already a bind there, otherwise make a new one
	if (con->binds.data[key] != NULL) {
		sdsclear(con->binds.data[key]);
	}
	else {
		con->binds.data[key] = sdsempty();
	}

	con->binds.data[key] = sdscat(con->binds.data[key], value);
}

// removes a bind by freeing the string and resetting it to null so it can be properly detected as not bound
void Con_RemoveBind(int key) {
	if (key < 0 || key > con->binds.capacity) {
		return;
	}

	if (con->binds.data[key] != NULL) {
		sdsfree(con->binds.data[key]);
		con->binds.data[key] = NULL;
	}
}

// sets up button commands, which are commands prefixed with a +.
void Con_AllocateButtons(const char **buttonNames, int buttonCount) {
	// free the strings of the existing structures before clearing the array
	if (con->buttons.length) {
		int i;
		buttonState_t button;
		vec_foreach(&con->buttons, button, i) {
			sdsfree(button.name);
		}
	}

	vec_clear(&con->buttons);

	// remake the entries using the new string values
	for (int i = 0; i < buttonCount; i++) {
		buttonState_t newButton = { 0 };
		newButton.name = sdsnew(buttonNames[i]);
		vec_push(&con->buttons, newButton);
	}
}

// returns a pointer to a button. pointer should be stable as long as you don't call
// Con_AllocateButtons a second time.
buttonState_t *Con_GetButton(int buttonNum) {
	if (buttonNum < 0 || buttonNum > con->buttons.length) {
		return NULL;
	}

	return &con->buttons.data[buttonNum];
}