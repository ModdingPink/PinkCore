#include "CustomTypes/RequirementModalListTableData.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "assets.hpp"
#include "System/Action_3.hpp"
#include "questui/shared/CustomTypes/Components/MainThreadScheduler.hpp"

#include "GlobalNamespace/LayoutWidthLimiter.hpp"
#include "UnityEngine/Resources.hpp"
#include "HMUI/TableView_ScrollPositionType.hpp"
#include "custom-types/shared/delegate.hpp"
#include "logging.hpp"
DEFINE_TYPE(PinkCore::UI, RequirementModalListTableData);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

namespace PinkCore::UI
{
    void RequirementModalListTableData::ctor()
    {
        reuseIdentifier = "RequirementModalListTableCell";
        cellSize = 8.5f;
        missingSpriteSprite = ArrayToSprite(Assets::Requirements::MissingSprite_png);
        requirementFoundSprite = ArrayToSprite(Assets::Requirements::RequirementFound_png);
        requirementMissingSprite = ArrayToSprite(Assets::Requirements::RequirementMissing_png);
        suggestionFoundSprite = ArrayToSprite(Assets::Requirements::SuggestionFound_png);
        suggestionMissingSprite = ArrayToSprite(Assets::Requirements::SuggestionMissing_png);
        infoSprite = ArrayToSprite(Assets::Requirements::Info_png);
        wipSprite = ArrayToSprite(Assets::Requirements::WIP_png);
        coloursSprite = ArrayToSprite(Assets::Requirements::Colors_png);
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetTableCell()
    {
        auto tableCell = reinterpret_cast<GlobalNamespace::LevelListTableCell*>(tableView->DequeueReusableCellForIdentifier(reuseIdentifier));
        if (!tableCell)
        {
            if (!songListTableCellInstance)
                songListTableCellInstance = ArrayUtil::First(Resources::FindObjectsOfTypeAll<GlobalNamespace::LevelListTableCell*>(), [](auto x){ return x->get_name() == u"LevelListTableCell"; });

            tableCell = Instantiate(songListTableCellInstance);
        }

        tableCell->notOwned = false;

        tableCell->set_reuseIdentifier(reuseIdentifier);
        tableCell->set_interactable(true);

        tableCell->songBpmText->get_gameObject()->SetActive(false);
        tableCell->songDurationText->get_gameObject()->SetActive(false);
        tableCell->favoritesBadgeImage->get_gameObject()->SetActive(false);
        static auto BpmIcon = ConstString("BpmIcon");

        // new stuff in 1.28.0 that needs to be disabled
        tableCell->updatedBadgeGo->SetActive(false);
        tableCell->promoBadgeGo->SetActive(false);
        tableCell->promoBackgroundGo->SetActive(false);
        tableCell->layoutWidthLimiter->set_limitWidth(false);

        tableCell->get_transform()->Find(BpmIcon)->get_gameObject()->SetActive(false);
        reinterpret_cast<RectTransform*>(tableCell->songNameText->get_transform())->set_anchorMax({2, 0.5});
        reinterpret_cast<RectTransform*>(tableCell->songAuthorText->get_transform())->set_anchorMax({2, 0.5});
        tableCell->selectedBackgroundColor = tableCell->highlightBackgroundColor;
        tableCell->selectedAndHighlightedBackgroundColor = tableCell->highlightBackgroundColor;
        /*
        INFO("creating the funny Sc2bad delegate");
        std::function<void(HMUI::SelectableCell *, HMUI::SelectableCell::TransitionType, Il2CppObject*)> fun = [tableCell](HMUI::SelectableCell* a, HMUI::SelectableCell::TransitionType b, Il2CppObject* c){

        };
        auto delegate = custom_types::MakeDelegate<System::Action_3<HMUI::SelectableCell *, HMUI::SelectableCell::TransitionType, Il2CppObject*>*>(classof(System::Action_3<HMUI::SelectableCell *, HMUI::SelectableCell::TransitionType, Il2CppObject*>*), fun);
        INFO("delegate made");
        tableCell->add_selectionDidChangeEvent(delegate);
        INFO("delegate added to did change event");
        */
        return tableCell;
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetWipCell()
    {
        auto tableCell = GetTableCell();
        tableCell->set_interactable(false);
        static auto WIPLevel = ConstString("WIP Level");
        static auto WorkInProgress = ConstString("Please Play in Practice Mode");
        tableCell->songNameText->set_text(WIPLevel);
        tableCell->songAuthorText->set_text(WorkInProgress);
        tableCell->coverImage->set_sprite(wipSprite);
        return tableCell;
    }


    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetCustomColoursCell()
    {
        auto tableCell = GetTableCell();
        tableCell->set_interactable(true);
        static auto colourAvailable = ConstString("Custom Colours Available");
        static auto previewColour = ConstString("Click here to preview & enable them.");
        tableCell->songNameText->set_text(colourAvailable);
        tableCell->songAuthorText->set_text(previewColour);
        tableCell->coverImage->set_sprite(coloursSprite);
        return tableCell;
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetRequirementOrSuggestionCell(std::string requirementName)
    {
        auto tableCell = GetTableCell();
        tableCell->set_interactable(false);

        static auto RequirementFound = ConstString("Requirement Found");
        static auto RequirementMissing = ConstString("Requirement Missing");
        static auto SuggestionFound = ConstString("Suggestion Found");
        static auto SuggestionMissing = ConstString("Suggestion Missing");

        bool installed = RequirementUtils::GetRequirementInstalled(requirementName);
		bool required = RequirementUtils::GetSongHasRequirement(requirementName);
		bool suggested = RequirementUtils::GetSongHasSuggestion(requirementName);
		bool forcedSuggestion = RequirementUtils::GetIsForcedSuggestion(requirementName);

        UnityEngine::Sprite* sprite;
        StringW subText;

		if (required && (!forcedSuggestion || suggested))
        {
            if (installed)
            {
                sprite = requirementFoundSprite;
                subText = RequirementFound;
            }
            else
            {
                sprite = requirementMissingSprite;
                subText = RequirementMissing;
            }
        }
        // it was a suggestion or forced suggestion
        else
        {
            if (installed)
            {
                sprite = suggestionFoundSprite;
                subText = SuggestionFound;
            }
            else
            {
                sprite = suggestionMissingSprite;
                subText = SuggestionMissing;
            }
        }

        tableCell->songNameText->set_text(requirementName);
        tableCell->songAuthorText->set_text(subText);
        tableCell->coverImage->set_sprite(sprite);

        return tableCell;
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetRequirementCell(int idx)
    {
        return GetRequirementOrSuggestionCell(SongUtils::SongInfo::get_mapData().currentRequirements[idx]);
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetSuggestionCell(int idx)
    {
        return GetRequirementOrSuggestionCell(SongUtils::SongInfo::get_mapData().currentSuggestions[idx]);
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetContributorCell(int idx)
    {
        auto tableCell = GetTableCell();
        auto& contributor = SongUtils::SongInfo::get_mapData().currentContributors[idx];
        tableCell->set_interactable(false);
        if (contributor.iconPath.empty()) {
            tableCell->songNameText->set_text(contributor.name);
            tableCell->songAuthorText->set_text(contributor.role);
            tableCell->coverImage->set_sprite(infoSprite);
        } else {
            tableCell->songNameText->set_text(contributor.name);
            tableCell->songAuthorText->set_text(contributor.role);
            tableCell->coverImage->set_sprite(UIUtils::FileToSprite(SongUtils::GetCurrentSongPath() + u"/" + contributor.iconPath));
        }

        return tableCell;
    }

    void RequirementModalListTableData::Refresh()
    {
        tableView->ReloadData();
        tableView->RefreshCells(true, true);
        tableView->ScrollToCellWithIdx(0, HMUI::TableView::ScrollPositionType::Beginning, true);
    }

    HMUI::TableCell* RequirementModalListTableData::CellForIdx(HMUI::TableView* tableView, int idx)
    {
        auto mapData = SongUtils::SongInfo::get_mapData();
        if (mapData.hasCustomColours)
        {
            if (idx == 0) return GetCustomColoursCell();
            else idx--;
        }

        if (idx < mapData.currentRequirements.size())
        {
            return GetRequirementCell(idx);
        }
        else
        {
            idx -= mapData.currentRequirements.size();
        }

        if (idx < mapData.currentSuggestions.size())
        {
            return GetSuggestionCell(idx);
        }
        else
        {
            idx -= mapData.currentSuggestions.size();
        }

        if (SongUtils::SongInfo::get_mapData().isWIP)
        {
            if (idx == 0) return GetWipCell();
            else idx--;
        }

        return GetContributorCell(idx);
    }


    float RequirementModalListTableData::CellSize()
    {
        return cellSize;
    }

    int RequirementModalListTableData::NumberOfCells()
    {
        // iswip is 0 or 1, requirements size, suggestions size, contributor size
        auto mapData = SongUtils::SongInfo::get_mapData();
        return mapData.isWIP + mapData.hasCustomColours + mapData.currentRequirements.size() + mapData.currentSuggestions.size() + mapData.currentContributors.size();
    }
}
