#include "wren/wren.hpp"
extern "C" {
#include "wren/wren_vm.h"
#include "wren/wren_debug.h"
}
#include <imgui.h>

ClassInfo *findClass(WrenVM* vm, ObjClass *cl);
void renderValue(WrenVM *vm, const char *name, Value value);
void renderInstance(WrenVM *vm, Value value);

static ImColor labelColor = ImColor(128, 128, 128);
static ImColor valueColor = ImColor(255, 255, 255);
static ImColor treeColor = ImColor(128, 255, 128);
static ImColor methodColor = ImColor(255, 255, 0);

ClassInfo *findClass(WrenVM* vm, ObjClass *cl) {
	auto hash = cl->name->hash;
	for (int i = 0; i < vm->classInfoCount; i++) {
		auto ci = &vm->classInfo[i];
		if (hash == ci->name->hash) {
			return ci;
		}
	}

	return nullptr;
}

void renderValue(WrenVM *vm, const char *name, Value value) {
	if (IS_BOOL(value)) {
		auto b = AS_BOOL(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "%s", b ? "true" : "false");
	}
	else if (IS_CLASS(value)) {
		auto c = AS_CLASS(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "(class %s)", c->name->value);
	}
	else if (IS_CLOSURE(value)) {
		auto cl = AS_CLOSURE(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "%s", "(closure)");
	}
	else if (IS_FIBER(value)) {
		auto f = AS_FIBER(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "%s", "(Fiber)");
	}
	else if (IS_FN(value)) {
		auto fi = AS_FN(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "(Fn arity %i)", fi->arity);
	}
	else if (IS_FOREIGN(value)) {
		auto fo = AS_FOREIGN(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "(foreign %s)", fo->obj.classObj->name->value);
	}
	else if (IS_INSTANCE(value)) {
		auto in = AS_INSTANCE(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		if (ImGui::TreeNode((const void*)value, "%s: %s", name, in->obj.classObj->name->value)) {
			renderInstance(vm, value);
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();

	}
	else if (IS_NULL(value)) {
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "(null)");
	}
	else if (IS_UNDEFINED(value)) {
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "(undefined)");
	}
	else if (IS_NUM(value)) {
		auto num = AS_NUM(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "%g", num);
	}
	else if (IS_MAP(value)) {
		auto m = AS_MAP(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
		if (ImGui::TreeNode((const void*)value, "%s: Map (%i)", name, m->count)) {
			for (int i = 0; i < m->count; i++) {
				if (m->entries[i].key == UNDEFINED_VAL) {
					continue;
				}

				const char *key = "(non-string key)";
				if (IS_STRING(m->entries[i].key)) {
					key = AS_CSTRING(m->entries[i].key);
				}
				else {
					auto obj = AS_OBJ(m->entries[i].key);
					if (obj != nullptr) {
						key = obj->classObj->name->value;
					}
				}
				renderValue(vm, key, m->entries[i].value);
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}
	else if (IS_STRING(value)) {
		auto str = AS_CSTRING(value);
		ImGui::TextColored(labelColor, "%s:", name);
		ImGui::SameLine();
		ImGui::TextColored(valueColor, "\"%s\"", str);
	}
	else if (IS_LIST(value)) {
		auto l = AS_LIST(value);
		ImGui::PushStyleColor(ImGuiCol_Text, treeColor);
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
}

void renderInstance(WrenVM *vm, Value value) {
	auto *obj = AS_OBJ(value);
	if (obj->type != OBJ_INSTANCE) {
		return;
	}

	auto *instance = AS_INSTANCE(value);

	ClassInfo *classInfo = findClass(vm, obj->classObj);

	if (classInfo == nullptr) {
		ImGui::Text("could not find classinfo for %s\n", obj->classObj->name->value);
		return;
	}

	ClassInfo *parentClassInfo = findClass(vm, obj->classObj->superclass);

	if (obj->classObj->superclass->superclass != nullptr) {
		ImColor red = ImColor(255, 0, 0);
		ImGui::TextColored(red, "WARNING: multiple superclasses");
		ImGui::TextColored(red, "fields may be misaligned");
	}

	if (obj->classObj->methods.count > 0) {
		ImGui::PushStyleColor(ImGuiCol_Text, methodColor);
		if (ImGui::TreeNode((const void*)&obj->classObj->methods, "Methods")) {
			for (int i = 0; i < obj->classObj->methods.count; i++) {
				auto method = obj->classObj->methods.data[i];
				if (method.type != METHOD_NONE && method.type != METHOD_PRIMITIVE) {
					ImGui::Text("%s", vm->methodNames.data[i].buffer);
				}

			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor();
	}

	for (int i = 0; i < obj->classObj->numFields; i++) {
		auto field = instance->fields[i];
		const char *name;

		int parentFieldCount = parentClassInfo ? parentClassInfo->fields.count : 0;
		if (parentClassInfo && i < parentFieldCount) {
			name = parentClassInfo->fields.data[i].buffer;
		}
		else if (i - parentFieldCount >= classInfo->fields.count) {
			name = "???";
		}
		else {
			name = classInfo->fields.data[i - parentFieldCount].buffer;
		}

		renderValue(vm, name, field);
	}
}

void wren_trap_inspect_instance(WrenVM *vm) {
	auto *obj = AS_OBJ(vm->apiStack[1]);

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImColor(0, 0, 0, 255));
	char windowTitle[64] = "";
	snprintf(windowTitle, 64, "%s (%p)", obj->classObj->name->value, obj);
	ImGui::Begin(windowTitle, nullptr, 0);
	if (IS_INSTANCE(vm->apiStack[1])) {
		renderInstance(vm, vm->apiStack[1]);
	}
	else {
		renderValue(vm, obj->classObj->name->value, vm->apiStack[1]);
	}

	ImGui::End();
	ImGui::PopStyleColor();
}