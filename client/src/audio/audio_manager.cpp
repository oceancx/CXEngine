#include "audio_manager.h"
#include "lua_bind.h"
#include "tsv.h"
#include "resource_manager.h"
#include <actor\actor_manager.h>

#include <extras/stb_vorbis.c>
#define  DR_FLAC_IMPLEMENTATION
#include <extras/dr_flac.h>  /* enables flac decoding. */
#define DR_MP3_IMPLEMENTATION 
#include <extras/dr_mp3.h>   /* enables mp3 decoding. */
#define DR_WAV_IMPLEMENTATION 
#define DR_WAVE_FORMAT_GSM_IMPLEMENTATION
#include <extras/dr_wav.h>   /* enables wav decoding. */

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

struct AudioFile {
	ma_device device;
	ma_decoder decoder;
	CXString path;
	bool loop;
	bool erase;
	bool start;
};
bool g_BGMSwithOn = true;
std::deque<AudioFile*> g_AudioFiles;
void audio_manager_clear();
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
	AudioFile* audioFile = (AudioFile*)pDevice->pUserData;
	if (audioFile == NULL) {
		return;
	}
	
	if (!audioFile->erase && g_BGMSwithOn) {
		ma_decoder* pDecoder = &audioFile->decoder;
		ma_uint64 read = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount);
		if (read < frameCount) {
			if (audioFile->loop) {
				ma_decoder_seek_to_pcm_frame(pDecoder, 0);
			}
			else {
				pDevice->pUserData = nullptr;
				audioFile->erase = true;
			}
		}
	}

	(void)pInput;
}

void audio_manager_stop(const char* path) {
	for (auto it = g_AudioFiles.begin(); it != g_AudioFiles.end(); it++) {
		if ((*it)->path == path) {
			(*it)->erase = true;
		}
	}
}

void audio_manager_clear() {
	for (auto& it : g_AudioFiles) {
		it->erase = true;
	}
}

int audio_manager_play(const char* path, bool loop)
{
	if (strcmp(path, "") == 0 || !g_BGMSwithOn)return -1;
	if (g_AudioFiles.size() > 0) {
		for (auto it = g_AudioFiles.begin(); it != g_AudioFiles.end();) {
			if ((*it)->erase) {
				ma_device_uninit(&(*it)->device);
				ma_decoder_uninit(&(*it)->decoder);
				delete (*it);
				it = g_AudioFiles.erase(it);
			}
			else {
				it++;
			}
		}
	}

	AudioFile* file = new AudioFile();
	file->erase = false;
	file->loop = loop;
	file->start = false;
	file->path = path;
	g_AudioFiles.push_back(file);

	ma_device& device = file->device;
	ma_decoder& decoder = file->decoder;

	auto residsplits = utils::split_by_cnt(path, '-', 2);
	uint32 pack_index = std::stoul(residsplits[0], 0);
	uint32 wasID = std::stoul(residsplits[1], 0, 16);
	uint64_t resid = res_encode_was(pack_index, wasID);
	uint8_t* pData;
	size_t size;
	int type = RESOURCE_MANAGER_INSTANCE->LoadWDFData(resid, pData, size);
	
	ma_result result = ma_decoder_init_memory(pData, size, NULL, &decoder);
	if (result != MA_SUCCESS) {
		return -2;
	}
	ma_device_config deviceConfig;
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = file;

	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		printf("Failed to open playback device.\n");
		ma_decoder_uninit(&decoder);
		return -3;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		printf("Failed to start playback device.\n");
		ma_device_uninit(&device);
		ma_decoder_uninit(&decoder);
		return -4;
	}

	return 0;
};

#ifdef L
#undef L
#endif // L

int lua_audio_manager_play(lua_State* L){
	const char* path = lua_tostring(L, 1);
	bool loop = lua_toboolean(L, 2);
	audio_manager_play(path, loop);
	return 0;
}

void audio_manager_toggle_bgm(){
	g_BGMSwithOn = !g_BGMSwithOn;
}
bool audio_manager_is_bgm_on() {
	return g_BGMSwithOn;
}


void luaopen_audio_manager(lua_State* L)
{
	script_system_register_luac_function_with_name(L,"audio_manager_play", lua_audio_manager_play);
	script_system_register_function(L, audio_manager_stop);
	script_system_register_function(L, audio_manager_clear);
	script_system_register_function(L, audio_manager_toggle_bgm);
	script_system_register_function(L, audio_manager_is_bgm_on);
	
}
