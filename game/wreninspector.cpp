#include "wren/wren.hpp"
extern "C" {
#include "wren/wren_vm.h"
#include "wren/wren_debug.h"
}
#include <imgui.h>
#include <math.h>
#include <stdio.h>

// prototypes
static ClassInfo *findClass(WrenVM* vm, ObjClass *cl);
static bool renderValue(WrenVM *vm, const char *name, Value value, bool enableClick = false);
static void renderInstance(WrenVM *vm, Value value);

// colors for imgui
static ImVec4 labelColor = (ImVec4) ImColor(128, 128, 128);
static ImVec4 valueColor = (ImVec4) ImColor(255, 255, 255);
static ImVec4 treeColor = (ImVec4) ImColor(128, 255, 128);
static ImVec4 methodColor = (ImVec4) ImColor(255, 255, 0);

// loop through our list of stored classes that we hacked out of the compiler
// this could probably be improved by searching per module, or just storing
// the field names on the classes directly, but that was hard.
ClassInfo *findClass(WrenVM* vm, ObjClass *cl) {
	if (cl == nullptr) {
		return nullptr;
	}
	uint32_t hash = cl->name->hash;
	for (int i = 0; i < vm->classInfoCount; i++) {
		ClassInfo *ci = &vm->classInfo[i];
		if (hash == ci->name->hash) {
			return ci;
		}
	}

	return nullptr;
}

// value editor
enum {
	EDIT_NUM,
	EDIT_STRING,
	EDIT_BOOL,
	EDIT_NULL
};

static int selectedFieldNum = -1;
static int selectedType = 0;

static void renderEditor(WrenVM *vm, ObjInstance *instance) {
	static float newFloat;
	static const size_t newStrSz = 64;
	static char newStr[64];
	static bool newBool;

	if (ImGui::BeginPopup("setvalue")) {
		bool submitted = false;

		ImGui::PushItemWidth(75.0);
		ImGui::Combo("", &selectedType, "Num\0String\0Bool\0Null\0\0");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
			ImGui::SetKeyboardFocusHere(1);
		}

		if (selectedType == EDIT_NUM) {
			submitted = ImGui::InputFloat("###Float", &newFloat, 0, 0, -1, ImGuiInputTextFlags_EnterReturnsTrue);
		}
		else if (selectedType == EDIT_STRING) {
			submitted = ImGui::InputText("###String", newStr, newStrSz, ImGuiInputTextFlags_EnterReturnsTrue);
		}
		else if (selectedType == EDIT_BOOL) {
			ImGui::Checkbox("###Bool", &newBool);
		}

		ImGui::SameLine();

		if (submitted) {
			Value val;
			if (selectedType == EDIT_NUM) {
				val = NUM_VAL(newFloat);
			}
			else if (selectedType == EDIT_STRING) {
				val = wrenNewString(vm, newStr);
			}
			else if (selectedType == EDIT_BOOL) {
				val = BOOL_VAL(newBool);
			}
			else {
				val = NULL_VAL;
			}

			newFloat = 0;
			newStr[0] = '\0';
			newBool = false;

			instance->fields[selectedFieldNum] = val;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// basically a macro to setup the indention and formatting. the key is clickable
// to popup the value editor
static bool renderKey(const char *key, bool enableClick = false) {
	bool clicked = false;

	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_Text, labelColor);

	ImGui::TreeAdvanceToLabelPos();

	if (enableClick) {
		clicked = ImGui::SmallButton(key);
	}
	else {
		ImGui::Text(key);
	}

	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	return clicked;
}

// map entries can only be string, numbers, class objects, bools, range, or null.
// unused key entries will be undefined. this is mostly here to generate strings
// out of keys, and is a simpler verion of renderValue
static void renderMapEntry(WrenVM *vm, MapEntry *entry) {
	if (IS_UNDEFINED(entry->key)) {
		return;
	}

	char itemName[64] = "";
	const char *keyStr = "(unknown key)";
	if (IS_STRING(entry->key)) {
		keyStr = AS_CSTRING(entry->key);
	}
	else if (IS_NUM(entry->key)) {
		snprintf(itemName, 64, "%g", AS_NUM(entry->key));
		keyStr = itemName;
	}
	else if (IS_CLASS(entry->key)) {
		keyStr = AS_CLASS(entry->key)->name->value;
	}
	else if (IS_BOOL(entry->key)) {
		keyStr = AS_BOOL(entry->key) ? "true" : "false";
	}
	else if (IS_RANGE(entry->key)) {
		ObjRange *range = AS_RANGE(entry->key);
		snprintf(itemName, 64, "%g%s%g", range->from, range->isInclusive ? ".." : "...", range->to);
		keyStr = itemName;
	}
	else if (IS_NULL(entry->key)) {
		keyStr = "null";
	}

	renderValue(vm, keyStr, entry->value);
}

// render out a list of methods, used for Class values, and at the header of all Instance values
static void renderMethodBuffer(WrenVM *vm, MethodBuffer methods) {
	for (int i = 0; i < methods.count; i++) {
		Method *method = &methods.data[i];
		if (method->type != METHOD_NONE && method->type != METHOD_PRIMITIVE) {
			ImGui::TreeAdvanceToLabelPos();
			ImGui::Text("%s", vm->methodNames.data[i].buffer);
		}
	}
}

// render a field's value into imgui. this could be a switch except some types will
// not cast to an object so just do it the old fashioned way. there's some recursion
// here to handle items that will expand
static bool renderValue(WrenVM *vm, const char *name, Value value, bool enableClick) {
	bool keyClicked = false;
	if (IS_BOOL(value)) {
		auto b = AS_BOOL(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "%s", b ? "true" : "false");
	}

	else if (IS_CLASS(value)) {
		auto c = AS_CLASS(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// create a nested tree node, and recurse over the children
		bool treeActive = ImGui::TreeNode((const void*)value, "%s: (class %s)", name, c->name->value);
		ImGui::PopStyleColor();
		ImGui::PushStyleColor(ImGuiCol_Text, methodColor);
		if (treeActive) {
			renderMethodBuffer(vm, c->methods);
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	else if (IS_CLOSURE(value)) {
		auto cl = AS_CLOSURE(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "%s", "(closure)");
	}

	else if (IS_FIBER(value)) {
		auto f = AS_FIBER(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "%s", "(Fiber)");
	}

	else if (IS_FN(value)) {
		auto fi = AS_FN(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "(Fn arity %i)", fi->arity);
	}

	else if (IS_FOREIGN(value)) {
		auto fo = AS_FOREIGN(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "(foreign %s)", fo->obj.classObj->name->value);
	}

	else if (IS_INSTANCE(value)) {
		auto in = AS_INSTANCE(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// create a nested tree node, and recurse over the children
		if (ImGui::TreeNode((const void*)value, "%s: %s", name, in->obj.classObj->name->value)) {
			renderInstance(vm, value);
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	else if (IS_NULL(value)) {
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "(null)");
	}

	else if (IS_UNDEFINED(value)) {
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "(undefined)");
	}

	else if (IS_NUM(value)) {
		auto num = AS_NUM(value);
		keyClicked = renderKey(name, enableClick);
		// %g is weird with printing scientific notation, but will not print decimals if it's a whole number
		ImGui::TextColored(valueColor, (num == floorf(num)) ? "%g" : "%f", num);
	}

	else if (IS_MAP(value)) {
		auto m = AS_MAP(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		if (ImGui::TreeNode((const void*)value, "%s: Map", name)) {
			// loop through capacity and not count because map entries will be scattered
			for (int i = 0; i < m->capacity; i++) {
				MapEntry *entry = &m->entries[i];
				renderMapEntry(vm, entry);
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	else if (IS_STRING(value)) {
		auto str = AS_CSTRING(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "\"%s\"", str);
	}

	else if (IS_LIST(value)) {
		auto l = AS_LIST(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// print the list contents, and recurse 
		if (ImGui::TreeNode((const void*)value, "%s: List (%i)", name, l->elements.count)) {
			for (int i = 0; i < l->elements.count; i++) {
				char itemName[64] = "";
				snprintf(itemName, 64, "%i", i);
				renderValue(vm, itemName, l->elements.data[i]);
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	else if (IS_RANGE(value)) {
		auto range = AS_RANGE(value);
		keyClicked = renderKey(name, enableClick);
		ImGui::TextColored(valueColor, "%g%s%g", range->from, range->isInclusive ? ".." : "...", range->to);
	}

	return keyClicked;
}

// take a value, loop through all it's methods, display them, loop through all of it's fields, and display them.
// this can be recursively called from renderValue, but it's typically also the start of looping through something.
static void renderInstance(WrenVM *vm, Value value) {
	ObjInstance *instance = AS_INSTANCE(value);
	Obj *obj = &instance->obj;
	
	ClassInfo *classInfo = findClass(vm, obj->classObj);

	// this shouldn't happen but it might if you have > 64 classes since hardcoded limit from compiler hacks
	if (classInfo == nullptr) {
		ImGui::Text("could not find classinfo for %s\n", obj->classObj->name->value);
		return;
	}

	// print a tree for all the methods in this class. this *does* take into account superclasses.
	if (obj->classObj->methods.count > 0) {
		ImGui::PushStyleColor(ImGuiCol_Text, methodColor);
		// loop through all methods and print the name if its open, which also includes arity tokens
		if (ImGui::TreeNode((const void*)&obj->classObj->methods, "Methods")) {
			renderMethodBuffer(vm, obj->classObj->methods);
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	renderEditor(vm, instance);

	// loop through all the fields and print the value (mondo big function above)
	for (int i = 0; i < obj->classObj->numFields; i++) {
		Value field = instance->fields[i];

		// iterate through class heiarchy in reverse until we find the right class for the
		// field we are currently on. this is a hot path and seemingly pretty slow in debug.
		int fieldsLeft = obj->classObj->numFields - i;
		ClassInfo *currentClass = classInfo;
		ObjClass *currentObjClass = obj->classObj;

		// we still have fields to go
		while (currentObjClass != nullptr) {
			// consume the fields of the current class, see if we overflow
			fieldsLeft -= currentClass->fields.count;
			if (fieldsLeft <= 0) {
				break;
			}
			// still more fields to go, go up another level
			currentObjClass = currentObjClass->superclass;
			currentClass = findClass(vm, currentObjClass);
		}

		// we hopefully have the field name at this point. if we're negative, that's how many
		// fields we have progressed through the current class, so just * -1 to get the index
		const char *name = currentClass == nullptr ? "???" : currentClass->fields.data[fieldsLeft * -1].buffer;

		// just pass the value into a big function that will go through types
		bool keyClicked = renderValue(vm, name, field, true);

		if (keyClicked) {
			ImGui::OpenPopup("setvalue");
			selectedFieldNum = i;
			selectedType = IS_BOOL(field) ? EDIT_BOOL : IS_STRING(field) ? EDIT_STRING : IS_NULL(field) ? EDIT_NULL : EDIT_NUM;
		}
	}
}

void inspect(WrenVM *vm, Value val) {
	Obj *obj = AS_OBJ(val);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 255));

	char windowTitle[64] = "";
	snprintf(windowTitle, 64, "%s (%p)", obj->classObj->name->value, obj);
	if (ImGui::Begin(windowTitle, nullptr, 0)) {
		// if the top level object is an instance, don't draw a redundant node for it
		if (obj->type == OBJ_INSTANCE) {
			renderInstance(vm, val);
		}
		// just render the value (a list or a map is most common)
		else {
			renderValue(vm, obj->classObj->name->value, val);
		}
	}
	ImGui::End();

	ImGui::PopStyleColor();
}

void wren_trap_inspect(WrenVM *vm) {
	Value val = vm->apiStack[1];

	if (!IS_OBJ(val)) {
		ImGui::Begin("Inspect Error", nullptr, 0);
		ImGui::Text("Cannot inspect primitive types.");
		ImGui::End();
		return;
	}

	inspect(vm, val);
}