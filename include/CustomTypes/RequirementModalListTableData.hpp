#pragma once

#include "custom-types/shared/macros.hpp"

#include "HMUI/TableView.hpp"
#include "HMUI/TableCell.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Sprite.hpp"

#include "GlobalNamespace/LevelListTableCell.hpp"

DECLARE_CLASS_CODEGEN_INTERFACES(PinkCore::UI, RequirementModalListTableData, UnityEngine::MonoBehaviour, classof(HMUI::TableView::IDataSource*),
        DECLARE_INSTANCE_FIELD(GlobalNamespace::LevelListTableCell*, songListTableCellInstance);
        DECLARE_INSTANCE_FIELD(HMUI::TableView*, tableView);
        DECLARE_INSTANCE_FIELD(StringW, reuseIdentifier);
        DECLARE_INSTANCE_FIELD(float, cellSize);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, wipSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, coloursSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, requirementFoundSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, requirementMissingSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, suggestionFoundSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, suggestionMissingSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, missingSpriteSprite);
        DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, infoSprite);
        DECLARE_INSTANCE_METHOD(void, Awake);
        DECLARE_INSTANCE_METHOD(void, Refresh);
        DECLARE_INSTANCE_METHOD(void, SpriteInit);
        DECLARE_OVERRIDE_METHOD_MATCH(HMUI::TableCell*, CellForIdx, &HMUI::TableView::IDataSource::CellForIdx, HMUI::TableView* tableView, int idx);
        DECLARE_OVERRIDE_METHOD_MATCH(float, CellSize, &HMUI::TableView::IDataSource::CellSize);
        DECLARE_OVERRIDE_METHOD_MATCH(int, NumberOfCells, &HMUI::TableView::IDataSource::NumberOfCells);

        DECLARE_CTOR(ctor);

    private:
        GlobalNamespace::LevelListTableCell* GetTableCell();
        GlobalNamespace::LevelListTableCell* GetWipCell();
        GlobalNamespace::LevelListTableCell* GetCustomColoursCell();
        GlobalNamespace::LevelListTableCell* GetRequirementCell(int idx);
        GlobalNamespace::LevelListTableCell* GetSuggestionCell(int idx);
        GlobalNamespace::LevelListTableCell* GetContributorCell(int idx);
        GlobalNamespace::LevelListTableCell* GetRequirementOrSuggestionCell(std::string requirementName);

)
