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
        missingSpriteSprite = VectorToSprite(std::vector<uint8_t>(_binary_MissingSprite_png_start, _binary_MissingSprite_png_end));
        requirementFoundSprite = VectorToSprite(std::vector<uint8_t>(_binary_RequirementFound_png_start, _binary_RequirementFound_png_end));
        requirementMissingSprite = VectorToSprite(std::vector<uint8_t>(_binary_RequirementMissing_png_start, _binary_RequirementMissing_png_end));
        suggestionFoundSprite = VectorToSprite(std::vector<uint8_t>(_binary_SuggestionFound_png_start, _binary_SuggestionFound_png_end));
        suggestionMissingSprite = VectorToSprite(std::vector<uint8_t>(_binary_SuggestionMissing_png_start, _binary_SuggestionMissing_png_end));
        infoSprite = VectorToSprite(std::vector<uint8_t>(_binary_Info_png_start, _binary_Info_png_end));
        wipSprite = VectorToSprite(std::vector<uint8_t>(_binary_WIP_png_start, _binary_WIP_png_end));
        coloursSprite = VectorToSprite(std::vector<uint8_t>(_binary_Colors_png_start, _binary_Colors_png_end));
    }

    bool canInteract = false;

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
        tableCell->get_transform()->Find(StringW(BpmIcon))->get_gameObject()->SetActive(false);
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
        return GetRequirementOrSuggestionCell(RequirementUtils::GetCurrentRequirements()[idx]);
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetSuggestionCell(int idx)
    {
        return GetRequirementOrSuggestionCell(RequirementUtils::GetCurrentSuggestions()[idx]);
    }

    GlobalNamespace::LevelListTableCell* RequirementModalListTableData::GetContributorCell(int idx)
    {
        auto tableCell = GetTableCell();
        auto& contributor = ContributorUtils::GetContributors()[idx];

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
        canInteract = false;
        tableView->ReloadData();
        tableView->RefreshCells(true, true);
        tableView->ScrollToCellWithIdx(0, HMUI::TableView::ScrollPositionType::Beginning, true);
        canInteract = true;
    }

    HMUI::TableCell* RequirementModalListTableData::CellForIdx(HMUI::TableView* tableView, int idx)
    {

        if (SongUtils::SongInfo::get_currentlySelectedHasColours())
        {
            if (idx == 0) return GetCustomColoursCell();
            else idx--;
        }

        if (idx < RequirementUtils::GetCurrentRequirements().size())
        {
            return GetRequirementCell(idx);
        }
        else
        {
            idx -= RequirementUtils::GetCurrentRequirements().size();
        }

        if (idx < RequirementUtils::GetCurrentSuggestions().size())
        {
            return GetSuggestionCell(idx);
        }
        else
        {
            idx -= RequirementUtils::GetCurrentSuggestions().size();
        }

        if (SongUtils::SongInfo::get_currentlySelectedIsWIP())
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
        return SongUtils::SongInfo::get_currentlySelectedIsWIP() + SongUtils::SongInfo::get_currentlySelectedHasColours() + RequirementUtils::GetCurrentRequirements().size() + RequirementUtils::GetCurrentSuggestions().size() + ContributorUtils::GetContributors().size();
    }
}