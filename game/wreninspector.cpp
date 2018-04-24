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
static void renderValue(WrenVM *vm, const char *name, Value &value);
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
	EDIT_LIST,
	EDIT_RANGE,
	EDIT_NULL
};

static Value *selectedVal = nullptr;
static int selectedType = 0;

static void openEditor(Value *value) {
	selectedVal = value;
	selectedType = IS_BOOL(*value) ? EDIT_BOOL : IS_STRING(*value) ? EDIT_STRING : IS_RANGE(*value) ? EDIT_RANGE : IS_NULL(*value) ? EDIT_NULL : EDIT_NUM;
	ImGui::OpenPopup("setvalue");
}

static void renderEditor(WrenVM *vm) {
	static const size_t newStrSz = 64;
	static char newStr[64];
	static bool newBool;
	static int listSize;
	static int rangeFrom, rangeTo;
	static bool rangeInclusive;

	if (ImGui::BeginPopup("setvalue")) {
		assert(selectedVal != nullptr);

		bool submitted = false;

		ImGui::PushItemWidth(75.0);
		ImGui::Combo("", &selectedType, "Num\0String\0Bool\0List\0Range\0Null\0\0");
		ImGui::PopItemWidth();

		ImGui::SameLine();

		if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) {
			ImGui::SetKeyboardFocusHere(1);
		}

		switch (selectedType) {
		case EDIT_NUM:
			submitted = ImGui::InputText("##Num", newStr, newStrSz, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CharsDecimal);
			break;
		case EDIT_STRING:
			submitted = ImGui::InputText("##String", newStr, newStrSz, ImGuiInputTextFlags_EnterReturnsTrue);
			break;

		case EDIT_BOOL:
			ImGui::Checkbox("##Bool", &newBool);
			ImGui::SameLine();
			submitted = ImGui::Button("OK");
			break;

		case EDIT_LIST:
			ImGui::PushItemWidth(48);
			submitted = ImGui::InputInt("Size", &listSize, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopItemWidth();
			break;

		case EDIT_RANGE:
			ImGui::PushItemWidth(32);
			ImGui::InputInt("From", &rangeFrom, 0);
			ImGui::SameLine();
			ImGui::InputInt("To", &rangeTo, 0);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			ImGui::Checkbox("Inclusive", &rangeInclusive);
			ImGui::SameLine();
			submitted = ImGui::Button("OK");
			break;

		case EDIT_NULL:
			ImGui::SameLine();
			submitted = ImGui::Button("OK");
			break;
		}


		ImGui::SameLine();

		if (submitted) {
			Value newVal;
			switch (selectedType) {
			case EDIT_NUM: {
				double dbl = atof(newStr);
				newVal = NUM_VAL(dbl);
				break;
			}

			case EDIT_STRING:
				newVal = wrenNewString(vm, newStr);
				break;

			case EDIT_BOOL:
				newVal = BOOL_VAL(newBool);
				break;

			case EDIT_LIST: {
				ObjList *list = wrenNewList(vm, listSize);
				newVal = OBJ_VAL(list);
				for (int i = 0; i < list->elements.count; i++) {
					list->elements.data[i] = NULL_VAL;
				}
				break;
			}
				
			case EDIT_RANGE:
				newVal = wrenNewRange(vm, rangeFrom, rangeTo, rangeInclusive);
				break;

			default:
			case EDIT_NULL:
				newVal = NULL_VAL;
				break;
			}

			*selectedVal = newVal;
			ImGui::CloseCurrentPopup();

			selectedVal = nullptr;
			newStr[0] = '\0';
			newBool = rangeInclusive = false;
			rangeFrom = rangeTo = listSize = 0;

		}

		ImGui::EndPopup();
	}
}

// basically a macro to setup the indention and formatting. the key is clickable
// to popup the value editor
static void renderKey(const char *key, Value *val = nullptr) {
	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_Text, labelColor);

	ImGui::TreeAdvanceToLabelPos();

	if (val != nullptr) {
		if (ImGui::SmallButton(key)) {
			openEditor(val);
		}
	}
	else {
		ImGui::Text(key);
	}

	ImGui::PopStyleColor(3);
	ImGui::SameLine();
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
static void renderValue(WrenVM *vm, const char *name, Value &value) {
	if (IS_BOOL(value)) {
		auto b = AS_BOOL(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "%s", b ? "true" : "false");
	}

	else if (IS_CLASS(value)) {
		auto c = AS_CLASS(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// create a nested tree node, and recurse over the children
		bool treeActive = ImGui::TreeNode(name, "%s: (class %s)", name, c->name->value);
		ImGui::PopStyleColor();
		if (treeActive) {
			if (ImGui::SmallButton("Edit")) {
				openEditor(&value);
			}
			ImGui::PushStyleColor(ImGuiCol_Text, methodColor);
			renderMethodBuffer(vm, c->methods);
			ImGui::PopStyleColor();

			renderEditor(vm);

			ImGui::TreePop();
		}
	}

	else if (IS_CLOSURE(value)) {
		//auto cl = AS_CLOSURE(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "%s", "(closure)");
	}

	else if (IS_FIBER(value)) {
		//auto f = AS_FIBER(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "%s", "(Fiber)");
	}

	else if (IS_FN(value)) {
		auto fi = AS_FN(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "(Fn arity %i)", fi->arity);
	}

	else if (IS_FOREIGN(value)) {
		auto fo = AS_FOREIGN(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "(foreign %s)", fo->obj.classObj->name->value);
	}

	else if (IS_INSTANCE(value)) {
		auto in = AS_INSTANCE(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// create a nested tree node, and recurse over the children
		bool treeActive = ImGui::TreeNode(name, "%s: %s", name, in->obj.classObj->name->value);
		ImGui::PopStyleColor();
		if (treeActive) {
			if (ImGui::SmallButton("Edit")) {
				openEditor(&value);
			}
			renderInstance(vm, value);
			renderEditor(vm);
			ImGui::TreePop();
		}
	}

	else if (IS_NULL(value)) {
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "(null)");
	}

	else if (IS_UNDEFINED(value)) {
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "(undefined)");
	}

	else if (IS_NUM(value)) {
		auto num = AS_NUM(value);
		renderKey(name, &value);
		// %g is weird with printing scientific notation, but will not print decimals if it's a whole number
		ImGui::TextColored(valueColor, (num == floor(num)) ? "%g" : "%f", num);
	}

	else if (IS_MAP(value)) {
		auto m = AS_MAP(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		bool treeActive = ImGui::TreeNode(name, "%s: Map", name);
		ImGui::PopStyleColor();

		if (treeActive) {
			if (ImGui::SmallButton("Edit")) {
				openEditor(&value);
			}
			// loop through capacity and not count because map entries will be scattered
			for (uint32_t i = 0; i < m->capacity; i++) {
				MapEntry *entry = &m->entries[i];
				renderMapEntry(vm, entry);
			}
			renderEditor(vm);

			ImGui::TreePop();
		}
	}

	else if (IS_STRING(value)) {
		auto str = AS_CSTRING(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "\"%s\"", str);
	}

	else if (IS_LIST(value)) {
		auto l = AS_LIST(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		// print the list contents, and recurse 
		bool treeActive = ImGui::TreeNode(name, "%s: List (%i)", name, l->elements.count);
		ImGui::PopStyleColor();

		if (treeActive) {
			if (ImGui::SmallButton("Edit")) {
				openEditor(&value);
			}
			for (int i = 0; i < l->elements.count; i++) {
				char itemName[64] = "";
				snprintf(itemName, 64, "%i", i);
				renderValue(vm, itemName, l->elements.data[i]);
			}
			renderEditor(vm);
			ImGui::TreePop();
		}
	}

	else if (IS_RANGE(value)) {
		auto range = AS_RANGE(value);
		renderKey(name, &value);
		ImGui::TextColored(valueColor, "%g%s%g", range->from, range->isInclusive ? ".." : "...", range->to);
	}
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

	// loop through all the fields and print the value (mondo big function above)
	for (int i = 0; i < obj->classObj->numFields; i++) {
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
		renderValue(vm, name, instance->fields[i]);
	}
}

void inspect(WrenVM *vm, Value &val, const char *rootTitle, const char *windowTitle = nullptr) {
	if (windowTitle == nullptr) {
		windowTitle = "Inspector";
	}

	if (ImGui::Begin(windowTitle, nullptr, ImGuiWindowFlags_HorizontalScrollbar)) {
		if (IS_OBJ(val)) {
			Obj *obj = AS_OBJ(val);
			ImGui::PushID((void*)obj);
			renderValue(vm, rootTitle, val);
			renderEditor(vm);
			ImGui::PopID();
		}
		else {
			ImGui::Text("%s: Can't inspect primitive types.", rootTitle);
		}
	}
	ImGui::End();
}

void wren_trap_inspect(WrenVM *vm) {
	Value val = vm->apiStack[1];
	Value title = vm->apiStack[2];

	const char *windowTitle = IS_STRING(title) ? AS_CSTRING(title) : nullptr;

	char nodeTitle[64] = "";

	ObjFiber* fiber = vm->fiber;
	// grab module info for title (adapted from wrenDebugPrintStackTrace);
	for (int i = fiber->numFrames - 2; i >= 0; i--)
	{
		CallFrame* frame = &fiber->frames[i];
		ObjFn* fn = frame->closure->fn;
		if (fn->module == NULL) continue;
		if (fn->module->name == NULL) continue;

		// -1 because IP has advanced past the instruction that it just executed.
		int line = fn->debug->sourceLines.data[frame->ip - fn->code.data - 1];
		const char *moduleName = fn->module->name->value;
		snprintf(nodeTitle, 64, "%s:%i", moduleName, line);
		break;
	}

	inspect(vm, val, nodeTitle, windowTitle);
}