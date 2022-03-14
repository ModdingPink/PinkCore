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
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/BeatmapLevelDataExtensions.hpp"

#include "UnityEngine/UI/Button.hpp"
#include "logging.hpp"

void PreHandleRequirements(GlobalNamespace::IPreviewBeatmapLevel* level) {
    if (!level)
    {
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
            INFO("Info.dat read successful!");
            SongUtils::SongInfo::set_currentInfoDatValid(true);
            // Requirements and suggestions are called on RequirementUtils::HandleRequirementDetails();
            // TODO: Move this over there
        }
        else
        {
            SongUtils::SongInfo::set_currentInfoDatValid(false);
            RequirementUtils::onFoundRequirements().invoke(std::vector<std::string>{});
            RequirementUtils::onFoundSuggestions().invoke(std::vector<std::string>{});

            INFO("Info.dat read not successful!");
        }

        // if the level ID contains `WIP` then the song is a WIP song
        std::string levelIDString = level->get_levelID();
        bool isWIP = levelIDString.find("WIP") != std::string::npos;
        SongUtils::SongInfo::set_currentlySelectedIsWIP(isWIP);
    }
    else
    {
        SongUtils::SongInfo::set_currentInfoDatValid(false);
        RequirementUtils::onFoundRequirements().invoke(std::vector<std::string>{});
        RequirementUtils::onFoundSuggestions().invoke(std::vector<std::string>{});
    }
}

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
	StandardLevelDetailView_RefreshContent(self);
	auto beatmapCharacteristicSegmentedControlController = self->beatmapCharacteristicSegmentedControlController;
	auto selectedBeatmapCharacteristic = beatmapCharacteristicSegmentedControlController ? beatmapCharacteristicSegmentedControlController->selectedBeatmapCharacteristic : nullptr;
	auto serializedNameCS = selectedBeatmapCharacteristic ? selectedBeatmapCharacteristic->get_serializedName() : nullptr;
	std::u16string serializedName(serializedNameCS ? serializedNameCS : u"");

	SongUtils::SongInfo::set_lastPhysicallySelectedCharacteristic(serializedName);
	SongUtils::SongInfo::set_lastPhysicallySelectedDifficulty(SongUtils::GetDiffFromNumber(self->beatmapDifficultySegmentedControlController->selectedDifficulty));

    PreHandleRequirements(reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel *>(self->level));
	RequirementUtils::HandleRequirementDetails();
	ContributorUtils::FetchListOfContributors();

	UIUtils::SetupOrUpdateRequirementsModal(self);

	RequirementUtils::UpdatePlayButton();

    if (self->dyn__level() != nullptr && self->dyn__level()->get_beatmapLevelData() != nullptr) {
        self->dyn__beatmapDifficultySegmentedControlController()->SetData(GlobalNamespace::BeatmapLevelDataExtensions::GetDifficultyBeatmapSet(self->dyn__level()->get_beatmapLevelData(), self->dyn__beatmapCharacteristicSegmentedControlController()->get_selectedBeatmapCharacteristic())->get_difficultyBeatmaps(), self->dyn__beatmapDifficultySegmentedControlController()->get_selectedDifficulty());
    }
}

/* HACK: Not sure if this should be gone but I have no real other choice here since this is removed (is what it does still being done ?)
MAKE_AUTO_HOOK_MATCH(StandardLevelDetailViewController_UpdateActionButtonIntractability, &GlobalNamespace::StandardLevelDetailViewController::UpdateActionButtonIntractability, void, GlobalNamespace::StandardLevelDetailViewController* self) 
{
	// we hook this one because if we don't, the play button gets set to active at all times, even if we want it to be disabled
}
*/