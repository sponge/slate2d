#include <assert.h>
#include "assetloader.h"
#include "files.h"
#include "external/sds.h"
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_thread.h>
#include <soloud_speech.h>
#include <soloud_openmpt.h>
#include <imgui.h>

extern SoLoud::Soloud soloud;

void* Speech_Load(Asset &asset) {
	auto speech = new SoLoud::Speech();
	speech->setText(asset.path);

	return (void*)speech;
}

void Speech_Free(Asset &asset) {
	SoLoud::Speech* speech = (SoLoud::Speech*) asset.resource;
	delete speech;
}

void Speech_ParseINI(Asset &asset, ini_t *ini) {
	const char *text = ini_get(ini, asset.name, "text");

	if (text == nullptr) {
		Con_Errorf(ERR_FATAL, "asset %s is missing required field \"text\"", asset.name);
		return;
	}
	
	asset.path = sdsnew(text);
}

void* Sound_Load(Asset &asset) {
	unsigned char *musicbuf;
	auto sz = FS_ReadFile(asset.path, (void **)&musicbuf);

	if (sz <= 0) {
		return nullptr;
	}

	switch (asset.type) {
		case ASSET_SOUND: {
			auto sound = new SoLoud::Wav();
			sound->loadMem(musicbuf, sz, false, true);
			return (void*)sound;
		}

		case ASSET_MOD: {
			auto mod = new SoLoud::Openmpt();
			mod->loadMem(musicbuf, sz, false, true);
			return (void*)mod;
		}
	}

	return nullptr;
}

void Sound_Free(Asset &asset) {
	SoLoud::Wav* sound = (SoLoud::Wav*) asset.resource;
	delete sound;
}

void Mod_Free(Asset &asset) {
	SoLoud::Openmpt* mod = (SoLoud::Openmpt*) asset.resource;
	// this probably leaks but if openmpt fails to init then this will crash
	if (mod->mDataLen != 0) {
		delete mod;
	}
}

unsigned int Snd_Play(AssetHandle assetHandle, float volume, float pan, bool loop) {
	Asset* asset = Asset_Get(ASSET_ANY, assetHandle);

	if (asset->type != ASSET_SOUND && asset->type != ASSET_SPEECH && asset->type != ASSET_MOD) {
		Con_Error(ERR_GAME, "asset not valid");
		return 0;
	}

	if (asset->resource == nullptr) {
		Con_Error(ERR_GAME, "asset resource not valid");
		return 0;
	}

	SoLoud::AudioSource *src = (SoLoud::AudioSource*) asset->resource;
	src->setLooping(loop);
	return soloud.play(*src, volume, pan);
}

void Snd_Stop(unsigned int handle) {
	soloud.stop(handle);
}

void Snd_PauseResume(unsigned int handle, bool pause) {
	soloud.setPause(handle, pause);
}

void Sound_Inspect(Asset& asset, bool deselected) {
	static int busHandle;
	static int handle;
	static SoLoud::Bus bus;

	if (deselected) {
		soloud.stopAll();
		bus.stop();
		soloud.stop(busHandle);
		return;
	}

	bus.setVisualizationEnable(true);
	SoLoud::AudioSource *src = (SoLoud::AudioSource*) asset.resource;
	
	ImGui::Text("Channels: %i", src->mChannels);
	ImGui::Text("Sample Rate: %0.f", src->mBaseSamplerate);
	
	int voice = soloud.getVoiceFromHandle(handle);
	if (ImGui::Button(voice == -1 ? "Play" : "Stop")) {
		if (voice != -1) {
			bus.stop();
		} else {
			busHandle = soloud.play(bus);
			handle = bus.play(*src);
		}
	}

	float *fft = bus.calcFFT();
	ImGui::PlotHistogram("##FFT", fft, 256 / 2, 0, nullptr, 0, 20, ImVec2(0, 160), 8);
}