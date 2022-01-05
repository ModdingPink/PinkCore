#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"
#include "questui/shared/QuestUI.hpp"

#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "UI/Noticeboard.hpp"
#include "UI/Settings/PinkCoreFlowCoordinator.hpp"
#include "UI/Settings/PinkCoreDonationViewController.hpp"
#include "UI/Settings/PinkCoreFlowCoordinator.hpp"
#include "CustomTypes/RequirementElement.hpp"

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