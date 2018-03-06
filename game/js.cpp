#include "duktape/duktape.h"
extern "C" {
#include "duktape/duk_module_node.h"
}
#include "public.h"

duk_context *ctx;

#pragma region Native Bindings

static duk_ret_t trap_print(duk_context *ctx) {
	trap->Print("%s\n", duk_to_string(ctx, 0));
	return 0;
}

duk_ret_t cb_resolve_module(duk_context *ctx) {
	const char *module_id;
	const char *parent_id;

	module_id = duk_require_string(ctx, 0);
	parent_id = duk_require_string(ctx, 1);

	duk_push_sprintf(ctx, "scripts/%s.js", module_id);
	printf("resolve_cb: id:'%s', parent-id:'%s', resolve-to:'%s'\n",
		module_id, parent_id, duk_get_string(ctx, -1));

	return 1;
}

#pragma endregion

duk_ret_t cb_load_module(duk_context *ctx) {
	/*
	*  Entry stack: [ resolved_id exports module ]
	*/
	const char *resolved_id = duk_get_string(ctx, 0);

	char *script;
	int scriptSz = trap->FS_ReadFile(resolved_id, (void**)&script);
	if (scriptSz > 0) {
		duk_push_string(ctx, script);
		free(script);
		return 1;
	}
	else {
		return 0;
	}
}

void JS_Init() {
	ctx = duk_create_heap_default();

	duk_push_c_function(ctx, trap_print, 1 /*nargs*/);
	duk_put_global_string(ctx, "print");

	duk_push_object(ctx);
	duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "resolve");
	duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
	duk_put_prop_string(ctx, -2, "load");
	duk_module_node_init(ctx);

	char *script;
	int scriptSz = trap->FS_ReadFile("scripts/main.js", (void**)&script);
	if (scriptSz > 0) {
		duk_push_string(ctx, script);
		if (duk_peval(ctx) != 0) {
			printf("Script error: %s\n", duk_safe_to_string(ctx, -1));
		}
		duk_pop(ctx);
		free(script);
	}
}