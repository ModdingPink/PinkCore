#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

config_t config;

Configuration& getConfig() {
	static Configuration config({MOD_ID, VERSION, VERSION_LONG});
	config.Load();
	return config;
}

#define SAVE(name) \
	doc.AddMember(#name, config.name, allocator)

#define LOAD(name, method) \
	auto itr_ ##name = doc.FindMember(#name); \
	if (itr_ ##name != doc.MemberEnd()) { \
		config.name = itr_ ##name->value.method; \
	}else { foundEverything = false; }

void SaveConfig()
{
	INFO("Saving Configuration...");
	rapidjson::Document& doc = getConfig().config;
	doc.RemoveAllMembers();
	doc.SetObject();
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	SAVE(enableExtraSongDetails);
	SAVE(enableCustomSongColours);
	SAVE(forceNoteColours);
	SAVE(enableCustomDiffNames);
	SAVE(enableBurnMarks);
	SAVE(openToCustomLevels);
	SAVE(enableCustomCharacteristics);

	getConfig().Write();
	INFO("Saved Configuration!");
}

bool LoadConfig()
{
	INFO("Loading Configuration...");
	bool foundEverything = true;
	rapidjson::Document& doc = getConfig().config;

	LOAD(enableExtraSongDetails, GetBool());
	LOAD(enableCustomSongColours, GetBool());
	LOAD(enableCustomDiffNames, GetBool());
	LOAD(forceNoteColours, GetBool());
	LOAD(enableCustomCharacteristics, GetBool());
	LOAD(enableBurnMarks, GetBool());
	LOAD(openToCustomLevels, GetBool());

	if (!foundEverything)
	{
		ERROR("Configuration values were missing!");
		return false;
	}
	INFO("Loaded Configuration!");
	return true;
}
