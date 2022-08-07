#pragma once
#include <string>

typedef struct config_t {
	bool enableExtraSongDetails = true;
	bool forceNoteColours = false;
	bool enableCustomSongColours = true;
	bool enableCustomDiffNames = true;
	bool enableBurnMarks = true;
	bool openToCustomLevels = true;
} config_t;

extern config_t config;

bool LoadConfig();
void SaveConfig();
