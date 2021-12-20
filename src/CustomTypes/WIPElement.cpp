#include "CustomTypes/WIPElement.hpp"
#include "questui/shared/BeatSaberUI.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/UIUtils.hpp"

#include "static-defines.h"
#include "logging.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"

DEFINE_TYPE(PinkCore::UI, WIPElement);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

namespace PinkCore::UI
{
    void WIPElement::ctor()
    {
        inited = false;
        sprite = UIUtils::FileToSprite(wipImagePath);
    }

    void WIPElement::Init()
    {
        if (inited) return;
		inited = true;
        std::string name = "WIP";
		get_gameObject()->set_name(il2cpp_utils::newcsstr(name));

		HorizontalLayoutGroup* layout = GetComponent<HorizontalLayoutGroup*>();
		
		VerticalLayoutGroup* imageGroup = CreateVerticalLayoutGroup(layout->get_transform());
		auto layoutelem = imageGroup->get_gameObject()->AddComponent<LayoutElement*>();
		layoutelem->set_preferredWidth(12.0f);
		layoutelem->set_flexibleWidth(12.0f);
		layoutelem->set_minWidth(12.0f);

		layoutelem->set_preferredHeight(7.0f);
		layoutelem->set_flexibleHeight(7.0f);
		layoutelem->set_minHeight(7.0f);
		// buttons work better for displaying images, don't kill me
		Button* button = QuestUI::BeatSaberUI::CreateUIButton(imageGroup->get_transform(), "", "SettingsButton", [](){}); // no delegate needed, it's not interactable
		button->set_interactable(false);
		HMUI::ButtonSpriteSwap* spriteSwap = button->get_gameObject()->GetComponent<HMUI::ButtonSpriteSwap*>();
		spriteSwap->normalStateSprite = spriteSwap->highlightStateSprite = spriteSwap->pressedStateSprite = spriteSwap->disabledStateSprite = sprite;
		
		imageGroup->get_transform()->set_localScale(Vector3::get_one() * 0.25f);

		UnityEngine::UI::VerticalLayoutGroup* textGroup = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(layout->get_transform());
		layoutelem = textGroup->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredWidth(35.0f);

		layoutelem->set_preferredHeight(8.0f);
		layoutelem->set_flexibleHeight(8.0f);
		layoutelem->set_minHeight(8.0f);

		textGroup->set_spacing(-2.0f);
		textGroup->set_childAlignment(UnityEngine::TextAnchor::MiddleLeft);

		// which contributor is it? 
		auto nametext = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), "WIP Level");
		nametext->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		
		layoutelem = nametext->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		auto missingMessage = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), "Work In Progress");
		layoutelem = missingMessage->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		missingMessage->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		missingMessage->set_fontSize(missingMessage->get_fontSize() * 0.6f);
		missingMessage->set_color(Color::get_white() * 0.8f);
    }

    void WIPElement::CheckWipState()
    {
        get_gameObject()->SetActive(SongUtils::SongInfo::get_currentlySelectedIsWIP());
        get_transform()->SetAsFirstSibling();
    }
}