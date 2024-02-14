#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "custom-types/shared/register.hpp"
#include "bsml/shared/BSML.hpp"

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
#include "HMUI/SelectableCell.hpp"
#include "System/Action_3.hpp"
#include "hooks.hpp"
#include "logging.hpp"

#include "config.hpp"

#include "scotland2/shared/modloader.h"

modloader::ModInfo modInfo{MOD_ID, VERSION, VERSION_LONG};

extern "C" void setup(CModInfo* info) {
	info->id = MOD_ID;
	info->version = VERSION;
	info->version_long = VERSION_LONG;
}

extern "C" void late_load()
{
	Logger& logger = PinkCore::Logging::getLogger();
	logger.info("Loading pinkcore!");
	BSML::Init();

	// if config load fails, save the config so it will work next time
	if (!LoadConfig())
		SaveConfig();

	Hooks::InstallHooks(logger);
	custom_types::Register::AutoRegister();
	BSML::Register::RegisterMainMenu<PinkCore::UI::PinkCoreFlowCoordinator*>(MOD_ID, "");
}
