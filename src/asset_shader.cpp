#include "assetloader.h"
#include "external/rlgl.h"
#include "console/console.h"
#include "files.h"

void * Shader_Load(Asset & asset) {

	// shaders need to be setup before load.
	if (asset.resource == nullptr) {
		Com_Error(ERR_FATAL, "Shader_Load: shader not setup before load %s", asset.name);
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

	free(shasset->fs);
	free(shasset->vs);
	shasset->shader = shader;

	return (void*)shasset;
}

void Shader_Set(AssetHandle id, bool isFile, char *vs, char *fs) {
	Asset *asset = Asset_Get(ASSET_SHADER, id);

	if (asset == nullptr) {
		Com_Error(ERR_DROP, "Shader_Set: asset not found");
		return;
	}

	if (asset->loaded == true) {
		Com_Printf("WARNING: Shader_Set: trying to set already loaded asset\n");
		return;
	}

	auto shader = new ShaderAsset();

	shader->isFile = isFile;
	shader->fs = CopyString(fs);
	shader->vs = CopyString(vs);

	asset->resource = shader;
}

void Shader_Free(Asset & asset) {
	ShaderAsset *res = (ShaderAsset*)asset.resource;

	UnloadShader(*res->shader);

	delete res->shader;
	delete res;
}