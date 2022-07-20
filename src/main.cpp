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
//#include "CustomTypes/RequirementElement.hpp"

#include "GlobalNamespace/LevelCollectionNavigationController.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"

#include "hooks.hpp"
#include "logging.hpp"

#include "config.hpp"

ModInfo modInfo;

extern "C" void setup(ModInfo& info)
{
	info.id = ID;
	info.version = VERSION;

	modInfo = info;
}

extern "C" void load()
{
	il2cpp_functions::Class_Init(classof(HMUI::ImageView*));
    il2cpp_functions::Class_Init(classof(HMUI::CurvedTextMeshPro*));
	Logger& logger = PinkCore::Logging::getLogger();
	logger.info("Loading pinkcore!");
	QuestUI::Init();
	

	// if config load fails, save the config so it will work next time
	if (!LoadConfig())
		SaveConfig();
  
	Hooks::InstallHooks(logger);

	custom_types::Register::AutoRegister();

	QuestUI::Register::RegisterAllModSettingsFlowCoordinator<PinkCore::UI::PinkCoreFlowCoordinator*>({ID, VERSION});
}