#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "UI/Noticeboard.hpp"
#include "UI/Settings/PinkCoreFlowCoordinator.hpp"
#include "UI/Settings/PinkCoreDonationViewController.hpp"
#include "UI/Settings/PinkCoreFlowCoordinator.hpp"
#include "CustomTypes/RequirementElement.hpp"

#include "CustomTypes/CustomLevelInfoSaveData.hpp"

#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"

#include "Hooks.hpp"

#include "config.hpp"

std::vector<void (*)(Logger& logger)> Hooks::installFuncs;
ModInfo modInfo;

Logger& getLogger()
{
	static Logger* logger = new Logger(modInfo, LoggerOptions(false, true));
	return *logger;
}

MAKE_HOOK_MATCH(LevelCollectionNavigationController_HandleLevelCollectionViewControllerDidSelectLevel, &GlobalNamespace::LevelCollectionNavigationController::HandleLevelCollectionViewControllerDidSelectLevel, void, GlobalNamespace::LevelCollectionNavigationController* self, GlobalNamespace::LevelCollectionViewController* viewController, GlobalNamespace::IPreviewBeatmapLevel* level) 
{
	if (!level)
	{
		LevelCollectionNavigationController_HandleLevelCollectionViewControllerDidSelectLevel(self, viewController, level);
		return;
	}

	bool isCustom = SongUtils::SongInfo::isCustom(level);
	SongUtils::SongInfo::set_currentlySelectedIsCustom(isCustom);

	if (isCustom)
	{
		// clear current info dat
		std::shared_ptr<rapidjson::Document>& d = SongUtils::GetCurrentInfoDatPtr();
		if (SongUtils::CustomData::GetInfoJson(level, d))
		{
			SongUtils::SongInfo::set_currentInfoDatValid(true);
			getLogger().info("Info.dat read successful!");
		}
		else
		{
			SongUtils::SongInfo::set_currentInfoDatValid(false);
			getLogger().info("Info.dat read not successful!");
		}

		// if the level ID contains `WIP` then the song is a WIP song 
		std::string levelIDString = to_utf8(csstrtostr(level->get_levelID()));
		bool isWIP = levelIDString.find("WIP") != std::string::npos;
		SongUtils::SongInfo::set_currentlySelectedIsWIP(isWIP);
	}
	else
	{
		SongUtils::SongInfo::set_currentInfoDatValid(false);
	}

	LevelCollectionNavigationController_HandleLevelCollectionViewControllerDidSelectLevel(self, viewController, level);
}


// Implementation by https://github.com/StackDoubleFlow
MAKE_HOOK_MATCH(StandardLevelInfoSaveData_DeserializeFromJSONString, &GlobalNamespace::StandardLevelInfoSaveData::DeserializeFromJSONString, GlobalNamespace::StandardLevelInfoSaveData*, Il2CppString *stringData) {
	auto* original = StandardLevelInfoSaveData_DeserializeFromJSONString(stringData);
	if (!original) return nullptr;
	int origLength = original ? (original->difficultyBeatmapSets ? original->difficultyBeatmapSets->Length() : 0) : 0;
	::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet*> *customBeatmapSets = 
		::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet*>::NewLength(origLength);
	CustomJSONData::CustomLevelInfoSaveData* customSaveData = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomLevelInfoSaveData*>(original->songName, 
		original->songSubName, original->songAuthorName, original->levelAuthorName, original->beatsPerMinute, original->songTimeOffset, 
		original->shuffle, original->shufflePeriod, original->previewStartTime, original->previewDuration, original->songFilename, 
		original->coverImageFilename, original->environmentName, original->allDirectionsEnvironmentName, customBeatmapSets));
	
	std::string str = stringData ? to_utf8(csstrtostr(stringData)) : "{}";
	
	std::shared_ptr<rapidjson::Document> sharedDoc = std::make_shared<rapidjson::Document>();
	customSaveData->doc = sharedDoc;

	rapidjson::Document& doc = *sharedDoc;
	doc.Parse(str.c_str());

	if (doc.HasMember("_customData")) {
		customSaveData->customData = doc["_customData"];
	}

	rapidjson::Value& beatmapSetsArr = doc["_difficultyBeatmapSets"];

	for (rapidjson::SizeType i = 0; i < beatmapSetsArr.Size(); i++) {
		rapidjson::Value& beatmapSetJson = beatmapSetsArr[i];
		GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet *standardBeatmapSet = original->difficultyBeatmapSets->values[i];
		::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap*> *customBeatmaps = 
			::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap*>::NewLength(standardBeatmapSet->difficultyBeatmaps->Length());

		for (rapidjson::SizeType j = 0; j < standardBeatmapSet->difficultyBeatmaps->Length(); j++) {
			rapidjson::Value& difficultyBeatmapJson = beatmapSetJson["_difficultyBeatmaps"][j];
			GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap *standardBeatmap = standardBeatmapSet->difficultyBeatmaps->values[j];

			CustomJSONData::CustomDifficultyBeatmap *customBeatmap = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomDifficultyBeatmap*>(standardBeatmap->difficulty, 
				standardBeatmap->difficultyRank, standardBeatmap->beatmapFilename, standardBeatmap->noteJumpMovementSpeed, standardBeatmap->noteJumpStartBeatOffset));
			
			if (difficultyBeatmapJson.HasMember("_customData")) {
				customBeatmap->customData = difficultyBeatmapJson["_customData"];
			}

			customBeatmaps->values[j] = customBeatmap;
		}

		customBeatmapSets->values[i] = GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet::New_ctor(standardBeatmapSet->beatmapCharacteristicName, customBeatmaps);
	}

	return customSaveData;
}

extern "C" void setup(ModInfo& info)
{
	info.id = ID;
	info.version = VERSION;

	modInfo = info;
}

extern "C" void load()
{
	Logger& logger = getLogger();
	QuestUI::Init();
	

	// if config load fails, save the config so it will work next time
	if (!LoadConfig())
		SaveConfig();
  
	Hooks::InstallHooks(logger);

	custom_types::Register::AutoRegister();

	QuestUI::Register::RegisterModSettingsFlowCoordinator<PinkCore::UI::PinkCoreFlowCoordinator*>({ID, VERSION});

	getLogger().info("rank: %u", classof(PinkCore::UI::RequirementElement*)->rank);
}
  
void installMainHooks(Logger& logger)
{
	INSTALL_HOOK(logger, LevelCollectionNavigationController_HandleLevelCollectionViewControllerDidSelectLevel);
	INSTALL_HOOK(logger, StandardLevelInfoSaveData_DeserializeFromJSONString);
}
// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(installMainHooks)