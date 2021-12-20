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

#include "hooks.hpp"
#include "logging.hpp"

#include "config.hpp"

ModInfo modInfo;

MAKE_AUTO_HOOK_MATCH(LevelCollectionNavigationController_HandleLevelCollectionViewControllerDidSelectLevel, &GlobalNamespace::LevelCollectionNavigationController::HandleLevelCollectionViewControllerDidSelectLevel, void, GlobalNamespace::LevelCollectionNavigationController* self, GlobalNamespace::LevelCollectionViewController* viewController, GlobalNamespace::IPreviewBeatmapLevel* level) 
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
		auto& d = SongUtils::GetCurrentInfoDatPtr();
		if (SongUtils::CustomData::GetInfoJson(level, d))
		{
			SongUtils::SongInfo::set_currentInfoDatValid(true);
			INFO("Info.dat read successful!");
		}
		else
		{
			SongUtils::SongInfo::set_currentInfoDatValid(false);
			INFO("Info.dat read not successful!");
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
MAKE_AUTO_HOOK_MATCH(StandardLevelInfoSaveData_DeserializeFromJSONString, &GlobalNamespace::StandardLevelInfoSaveData::DeserializeFromJSONString, GlobalNamespace::StandardLevelInfoSaveData*, Il2CppString *stringData) {
	auto* original = StandardLevelInfoSaveData_DeserializeFromJSONString(stringData);
	if (!original) return nullptr;

	::ArrayW<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet*> customBeatmapSets = 
		::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet*>::NewLength(original->difficultyBeatmapSets.Length());
		
	CustomJSONData::CustomLevelInfoSaveData* customSaveData = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomLevelInfoSaveData*>(original->songName, 
		original->songSubName, original->songAuthorName, original->levelAuthorName, original->beatsPerMinute, original->songTimeOffset, 
		original->shuffle, original->shufflePeriod, original->previewStartTime, original->previewDuration, original->songFilename, 
		original->coverImageFilename, original->environmentName, original->allDirectionsEnvironmentName, customBeatmapSets));
	
	std::u16string str(stringData ? csstrtostr(stringData) : u"{}");
	
	std::shared_ptr<typename rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>> sharedDoc = std::make_shared<typename rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>();
	customSaveData->doc = sharedDoc;

	rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& doc = *sharedDoc;
	doc.Parse(str.c_str());

	auto dataItr = doc.FindMember(u"_customData");
	if (dataItr != doc.MemberEnd()) {
		customSaveData->customData = dataItr->value;
	}

	Value& beatmapSetsArr = doc.FindMember(u"_difficultyBeatmapSets")->value;
	
	INFO("beatmapSets length orig: %lu", original->difficultyBeatmapSets.Length());
	INFO("beatmapSets length json: %d", beatmapSetsArr.Size());

	for (rapidjson::SizeType i = 0; i < beatmapSetsArr.Size(); i++) {
		Value& beatmapSetJson = beatmapSetsArr[i];
		GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet *standardBeatmapSet = original->difficultyBeatmapSets[i];
		INFO("beatmapset: %p", standardBeatmapSet);
		INFO("standardBeatmapSet->difficultyBeatmaps: %p", (Il2CppArray*)standardBeatmapSet->difficultyBeatmaps);
		::ArrayW<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap*> customBeatmaps = 
			::Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap*>::NewLength(standardBeatmapSet->difficultyBeatmaps.Length());

		for (rapidjson::SizeType j = 0; j < standardBeatmapSet->difficultyBeatmaps.Length(); j++) {
			Value& difficultyBeatmapJson = beatmapSetJson.FindMember(u"_difficultyBeatmaps")->value[j];
			GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmap *standardBeatmap = standardBeatmapSet->difficultyBeatmaps[j];

			CustomJSONData::CustomDifficultyBeatmap *customBeatmap = CRASH_UNLESS(il2cpp_utils::New<CustomJSONData::CustomDifficultyBeatmap*>(standardBeatmap->difficulty, 
				standardBeatmap->difficultyRank, standardBeatmap->beatmapFilename, standardBeatmap->noteJumpMovementSpeed, standardBeatmap->noteJumpStartBeatOffset));
			
			auto customDataItr = difficultyBeatmapJson.FindMember(u"_customData");
			if (customDataItr != difficultyBeatmapJson.MemberEnd()) {
				customBeatmap->customData = customDataItr->value;
			}

			customBeatmaps[j] = customBeatmap;
		}

		customBeatmapSets[i] = GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet::New_ctor(standardBeatmapSet->beatmapCharacteristicName, customBeatmaps);
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
	Logger& logger = PinkCore::Logging::getLogger();
	logger.info("Loading pinkcore!");
	QuestUI::Init();
	

	// if config load fails, save the config so it will work next time
	if (!LoadConfig())
		SaveConfig();
  
	Hooks::InstallHooks(logger);

	custom_types::Register::AutoRegister();

	QuestUI::Register::RegisterModSettingsFlowCoordinator<PinkCore::UI::PinkCoreFlowCoordinator*>({ID, VERSION});

	INFO("rank: %u", classof(PinkCore::UI::RequirementElement*)->rank);
}