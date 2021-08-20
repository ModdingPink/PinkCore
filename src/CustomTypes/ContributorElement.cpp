#include "CustomTypes/ContributorElement.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "Utils/SongUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/UIUtils.hpp"

#include "HMUI/ButtonSpriteSwap.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/RectOffset.hpp"

DEFINE_TYPE(PinkCore::UI, ContributorElement);

using namespace UnityEngine;

namespace PinkCore::UI
{
	void ContributorElement::Init(Contributor& contributor)
	{
		if (inited) return;
		inited = true;
		this->contributor = contributor;

		std::string iconPath = SongUtils::GetCurrentSongPath() + "/" + contributor.iconPath;
		if (fileexists(iconPath))
			sprite = UIUtils::FileToSprite(iconPath);
		else
			sprite = nullptr;

		get_gameObject()->set_name(il2cpp_utils::newcsstr(contributor.name + contributor.role));

		UnityEngine::UI::HorizontalLayoutGroup* layout = GetComponent<UnityEngine::UI::HorizontalLayoutGroup*>();

		UnityEngine::UI::VerticalLayoutGroup* imageGroup = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(layout->get_transform());
		auto layoutelem = imageGroup->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredWidth(12.0f);
		layoutelem->set_flexibleWidth(12.0f);
		layoutelem->set_minWidth(12.0f);

		layoutelem->set_preferredHeight(8.0f);
		layoutelem->set_flexibleHeight(8.0f);
		layoutelem->set_minHeight(8.0f);
		
		// buttons work better for displaying images, don't kill me
		UnityEngine::UI::Button* button = QuestUI::BeatSaberUI::CreateUIButton(imageGroup->get_transform(), "", "SettingsButton", [](){}); // no delegate needed, it's not interactable
		button->set_interactable(false);

		HMUI::ButtonSpriteSwap* swap = button->GetComponent<HMUI::ButtonSpriteSwap*>();
		swap->normalStateSprite = sprite;
		swap->highlightStateSprite = sprite;
		swap->pressedStateSprite = sprite;
		swap->disabledStateSprite = sprite;

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
		auto name = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), contributor.name);
		name->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		
		layoutelem = name->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		auto role = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), contributor.role);
		layoutelem = role->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		role->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		role->set_fontSize(role->get_fontSize() * 0.6f);
		role->set_color(Color::get_white() * 0.8f);
	}

	void ContributorElement::ctor()
	{
		inited = false;
	}

	void ContributorElement::EnableObjectIfIsCurrentContributor()
	{
		get_gameObject()->SetActive(ContributorUtils::GetIsCurrentContributor(contributor));
	}

	bool ContributorElement::GetIsSame(Contributor& other)
	{
		return inited && (other == this->contributor);
	}
}