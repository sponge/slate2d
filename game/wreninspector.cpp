#include "wren/wren.hpp"
extern "C" {
#include "wren/wren_vm.h"
#include "wren/wren_debug.h"
}
#include <imgui.h>
#include <math.h>
#include <stdio.h>

// prototypes
ClassInfo *findClass(WrenVM* vm, ObjClass *cl);
void renderValue(WrenVM *vm, const char *name, Value value);
void renderInstance(WrenVM *vm, Value value);

// colors for imgui
static ImColor labelColor = ImColor(128, 128, 128);
static ImColor valueColor = ImColor(255, 255, 255);
static ImColor treeColor = ImColor(128, 255, 128);
static ImColor methodColor = ImColor(255, 255, 0);

// loop through our list of stored classes that we hacked out of the compiler
// this could probably be improved by searching per module, or just storing
// the field names on the classes directly, but that was hard.
ClassInfo *findClass(WrenVM* vm, ObjClass *cl) {
	if (cl == nullptr) {
		return nullptr;
	}
	auto hash = cl->name->hash;
	for (int i = 0; i < vm->classInfoCount; i++) {
		auto ci = &vm->classInfo[i];
		if (hash == ci->name->hash) {
			return ci;
		}
	}

	return nullptr;
}

// basically a macro to setup the indention and formatting
void renderKey(const char *key) {
	ImGui::TreeAdvanceToLabelPos();
	ImGui::TextColored(labelColor, "%s:", key);
	ImGui::SameLine();
}

// map entries can only be string, numbers, class objects, bools, range, or null.
// unused key entries will be undefined. this is mostly here to generate strings
// out of keys, and is a simpler verion of renderValue
void renderMapEntry(WrenVM *vm, MapEntry *entry) {
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
void renderMethodBuffer(WrenVM *vm, MethodBuffer methods) {
	for (int i = 0; i < methods.count; i++) {
		auto method = methods.data[i];
		if (method.type != METHOD_NONE && method.type != METHOD_PRIMITIVE) {
			ImGui::TreeAdvanceToLabelPos();
			ImGui::Text("%s", vm->methodNames.data[i].buffer);
		}
	}
}

// render a field's value into imgui. this could be a switch except some types will
// not cast to an object so just do it the old fashioned way. there's some recursion
// here to handle items that will expand
void renderValue(WrenVM *vm, const char *name, Value value) {
	if (IS_BOOL(value)) {
		auto b = AS_BOOL(value);
		renderKey(name);
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
		renderKey(name);
		ImGui::TextColored(valueColor, "%s", "(closure)");
	}

	else if (IS_FIBER(value)) {
		auto f = AS_FIBER(value);
		renderKey(name);
		ImGui::TextColored(valueColor, "%s", "(Fiber)");
	}

	else if (IS_FN(value)) {
		auto fi = AS_FN(value);
		renderKey(name);
		ImGui::TextColored(valueColor, "(Fn arity %i)", fi->arity);
	}

	else if (IS_FOREIGN(value)) {
		auto fo = AS_FOREIGN(value);
		renderKey(name);
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
		renderKey(name);
		ImGui::TextColored(valueColor, "(null)");
	}

	else if (IS_UNDEFINED(value)) {
		renderKey(name);
		ImGui::TextColored(valueColor, "(undefined)");
	}

	else if (IS_NUM(value)) {
		auto num = AS_NUM(value);
		renderKey(name);
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
		renderKey(name);
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
		renderKey(name);
		ImGui::TextColored(valueColor, "%g%s%g", range->from, range->isInclusive ? ".." : "...", range->to);
	}
}

void renderInstance(WrenVM *vm, Value value) {
	auto *obj = AS_OBJ(value);
	auto *instance = AS_INSTANCE(value);

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
		renderValue(vm, name, field);
	}
}

void wren_trap_inspect_instance(WrenVM *vm) {
	Value val = vm->apiStack[1];

	if (!IS_OBJ(val)) {
		ImGui::Begin("Inspect Error", nullptr, 0);
		ImGui::Text("Cannot inspect primitive types.");
		ImGui::End();
		return;
	}

	auto *obj = AS_OBJ(val);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0, 0, 0, 255));
	char windowTitle[64] = "";
	snprintf(windowTitle, 64, "%s (%p)", obj->classObj->name->value, obj);
	ImGui::Begin(windowTitle, nullptr, 0);
	// if the top level object is an instance, don't draw a redundant node for it
	if (IS_INSTANCE(val)) {
		renderInstance(vm, val);
	}
	// just render the value (a list or a map is most common)
	else {
		renderValue(vm, obj->classObj->name->value, val);
	}

	ImGui::End();
	ImGui::PopStyleColor();
}