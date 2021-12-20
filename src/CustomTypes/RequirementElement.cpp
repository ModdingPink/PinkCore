#include "CustomTypes/RequirementElement.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "static-defines.h"
#include "Utils/RequirementUtils.hpp"
#include "Utils/UIUtils.hpp"

#include "HMUI/ButtonSpriteSwap.hpp"
#include "logging.hpp"

DEFINE_TYPE(PinkCore::UI, RequirementElement);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

constexpr const char* hints[4] = { "<i>Requirement Found</i>", "<i>Requirement Missing</i>", "<i>Suggestion Found</i>", "<i>Suggestion Missing</i>" };
Il2CppString* hintsCS[4] = { nullptr, nullptr, nullptr, nullptr };
namespace PinkCore::UI
{   
	void RequirementElement::ctor()
	{
		inited = false;
		// static fields are now methods I guess
		if (!arrayCreated)
		{
			// assign to the returned reference
			arrayCreated = true;
			sprites = Array<Sprite*>::NewLength(4);
			for (int i = 0; i < 4; i++) sprites->values[i] = nullptr;
		}
	}

	void RequirementElement::Init(std::string name)
	{
		if (inited) return;
		inited = true;
		requirementName = name; 
		
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
		auto nametext = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), name);
		nametext->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		
		layoutelem = nametext->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		missingMessage = QuestUI::BeatSaberUI::CreateText(textGroup->get_transform(), "");
		layoutelem = missingMessage->get_gameObject()->AddComponent<UnityEngine::UI::LayoutElement*>();
		layoutelem->set_preferredHeight(0.3f);

		missingMessage->set_alignment(TMPro::TextAlignmentOptions::MidlineLeft);
		missingMessage->set_fontSize(missingMessage->get_fontSize() * 0.6f);
		missingMessage->set_color(Color::get_white() * 0.8f);
	}

	void RequirementElement::CheckRequirementState()
	{
		bool installed = RequirementUtils::GetRequirementInstalled(requirementName);
		bool required = RequirementUtils::GetSongHasRequirement(requirementName);
		bool suggested = RequirementUtils::GetSongHasSuggestion(requirementName);
		bool forcedSuggestion = RequirementUtils::GetIsForcedSuggestion(requirementName);

		INFO("Requirement %s is installed: %d, required: %d, suggested: %d", requirementName.c_str(), installed, required, suggested);

		/// if required and (not forced or is suggested)
		if (required && (!forcedSuggestion || suggested))
		{
			get_gameObject()->SetActive(true);
			if (installed)
				set_state(ReqFound);
			else 
				set_state(ReqMissing);
		} 
		// if suggested or (forced suggested and required)
		else if (suggested || (forcedSuggestion && required))
		{
			get_gameObject()->SetActive(true);
			if (installed)
				set_state(SugFound);
			else 
				set_state(SugMissing);
		}
		else get_gameObject()->SetActive(false);
	}

	void RequirementElement::set_state(RequirementState newstate)
	{
		state = newstate;
		// if not already loaded, load it
		if (!sprites->values[state]) 
		{
			std::string path = requirementImagePaths[state];
			sprites->values[state] = UIUtils::FileToSprite(path);
		}

		// set the correct sprite
		ButtonSpriteSwap* swap = GetComponentInChildren<Button*>()->GetComponent<HMUI::ButtonSpriteSwap*>();
		swap->normalStateSprite = sprites->values[state];
		swap->highlightStateSprite = sprites->values[state];
		swap->pressedStateSprite = sprites->values[state];
		swap->disabledStateSprite = sprites->values[state];

		// if specific part of the array is not initialized, init it
		if (!hintsCS[state]) 
		{
			hintsCS[state] = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>(hints[state]);
		}

		// set the text
		missingMessage->set_text(hintsCS[state]);
	}
}