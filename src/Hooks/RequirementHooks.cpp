#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"

#include "UnityEngine/UI/Button.hpp"

MAKE_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
	StandardLevelDetailView_RefreshContent(self);
	auto beatmapCharacteristicSegmentedControlController = self->beatmapCharacteristicSegmentedControlController;
	auto selectedBeatmapCharacteristic = beatmapCharacteristicSegmentedControlController ? beatmapCharacteristicSegmentedControlController->selectedBeatmapCharacteristic : nullptr;
	auto serializedNameCS = selectedBeatmapCharacteristic ? selectedBeatmapCharacteristic->get_serializedName() : nullptr;
	std::u16string serializedName(serializedNameCS ? csstrtostr(serializedNameCS) : u"");

	SongUtils::SongInfo::set_lastPhysicallySelectedCharacteristic(serializedName);
	SongUtils::SongInfo::set_lastPhysicallySelectedDifficulty(SongUtils::GetDiffFromNumber(self->beatmapDifficultySegmentedControlController->selectedDifficulty));

	RequirementUtils::HandleRequirementDetails();
	ContributorUtils::FetchListOfContributors();

	UIUtils::SetupOrUpdateRequirementsModal(self);

	RequirementUtils::UpdatePlayButton();
}

MAKE_HOOK_MATCH(StandardLevelDetailViewController_UpdateActionButtonIntractability, &GlobalNamespace::StandardLevelDetailViewController::UpdateActionButtonIntractability, void, GlobalNamespace::StandardLevelDetailViewController* self) 
{
	// we hook this one because if we don't, the play button gets set to active at all times, even if we want it to be disabled
}

void InstallRequirementHooks(Logger& logger)
{
	SIMPLE_INSTALL_HOOK(StandardLevelDetailView_RefreshContent);
	SIMPLE_INSTALL_HOOK(StandardLevelDetailViewController_UpdateActionButtonIntractability);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallRequirementHooks)