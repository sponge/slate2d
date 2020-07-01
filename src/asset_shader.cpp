#include "assetloader.h"
#include "external/rlgl.h"
#include "console.h"
#include "files.h"
#include <imgui.h>

void * Shader_Load(Asset & asset) {

	// shaders need to be setup before load.
	if (asset.resource == nullptr) {
		Con_Errorf(ERR_FATAL, "shader not setup before load %s", asset.name);
		return nullptr;
	}

	auto *shasset = (ShaderAsset*)asset.resource;

	// loadshader doesn't return a pointer so do a little weirdness here
	Shader *shader = new Shader();

	if (shasset->isFile) {
		char *vs;
		FS_ReadFile(shasset->vs, (void**)&vs);

		char *fs;
		FS_ReadFile(shasset->fs, (void**)&fs);

		*shader = LoadShaderCode(vs, fs);

		shasset->locResolution = GetShaderLocation(*shader, "iResolution");
		shasset->locTime = GetShaderLocation(*shader, "iTime");
		shasset->locTimeDelta = GetShaderLocation(*shader, "iTimeDelta");
		shasset->locMouse = GetShaderLocation(*shader, "iMouse");

		free(vs);
		free(fs);
	}
	else {
		*shader = LoadShaderCode(shasset->vs, shasset->fs);
	}

	shasset->shader = shader;

	return (void*)shasset;
}

void Shader_Set(AssetHandle id, bool isFile, const char *vs, const char *fs) {
	Asset *asset = Asset_Get(ASSET_SHADER, id);

	if (asset == nullptr) {
		Con_Error(ERR_GAME, "asset not found");
		return;
	}

	if (asset->loaded == true) {
		Con_Printf("WARNING: Shader_Set: trying to set already loaded asset\n");
		return;
	}

	auto shader = new ShaderAsset();

	shader->isFile = isFile;
	shader->fs = strdup(fs);
	shader->vs = strdup(vs);

	asset->resource = shader;
}

void Shader_Free(Asset & asset) {
	ShaderAsset *res = (ShaderAsset*)asset.resource;

	free((void*)res->fs);
	free((void*)res->vs);

	if (GetShaderDefault().id == res->shader->id) {
		Con_Print("not freeing default shader\n");
	} else {
		UnloadShader(*res->shader);
		delete res->shader;
	}

	delete res;
}

void Shader_ParseINI(Asset &asset, ini_t *ini) {
	const char *vs = ini_get(ini, asset.name, "vs");
	const char *fs = ini_get(ini, asset.name, "fs");

	Shader_Set(asset.id, true, vs, fs);
}

void Shader_Inspect(Asset& asset, bool deselected) {
	ShaderAsset *res = (ShaderAsset*)asset.resource;

	if (res->isFile) {
		ImGui::Text("Vertex Shader Path: %s", res->vs);
		ImGui::Text("Fragment Shader Path: %s", res->fs);
	} else {
		ImGui::Text("Vertex Shader:");
		ImGui::TextWrapped("%s", res->vs);
		ImGui::Text("Fragment Shader:");
		ImGui::TextWrapped("%s", res->fs);
	}
}