#include "logging.hpp"
#include "UI/Noticeboard.hpp"
#include "static-defines.h"
#include "assets.hpp"

#include "bsml/shared/BSML.hpp"
#include "bsml/shared/Helpers/creation.hpp"
#include "bsml/shared/BSML/Components/ExternalComponents.hpp"
#include "bsml/shared/BSML/Components/Backgroundable.hpp"
#include "bsml/shared/BSML/MainThreadScheduler.hpp"

#include "VRUIControls/VRGraphicRaycaster.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Component.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/RectOffset.hpp"

#include "Utils/NoticeBoardText.hpp"
#include "Utils/DonationText.hpp"
#include "Utils/UIUtils.hpp"
#include "TMPro/TMP_Text.hpp"

#include "HMUI/Touchable.hpp"

DEFINE_TYPE(PinkCore::UI, NoticeBoard);

using namespace BSML::Lite;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace TMPro;
using namespace HMUI;

constexpr const char* titles[2] = { "<i>NoticeBoard</i>", "<i>Patreon</i>" };

namespace PinkCore::UI
{
	NoticeBoard* NoticeBoard::instance = nullptr;

	void NoticeBoard::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
	{
		// if this is the first time this viewcontroller was activated
		if (firstActivation)
		{

			//parentViewController->
			INFO("NoticeBoard activated!");

			title = UIUtils::AddHeader(get_transform(), titles[state], Color(0.94f, 0.11f, 0.5f, 1.0f));
			CreateSwitchingButtons();

			container = CreateScrollableSettingsContainer(get_transform());
			auto components = container->GetComponent<BSML::ExternalComponents*>();
			RectTransform* rect = components->Get<RectTransform*>();
			rect->set_sizeDelta({0.0f, 0.0f});
			rect->set_anchoredPosition({0.0f,3.0f});
			// backgroundable for the text, gives it some borders
			auto* backgroundable = container->GetComponent<BSML::Backgroundable*>();
			if (!backgroundable) backgroundable = container->AddComponent<BSML::Backgroundable*>();
			backgroundable->ApplyBackground("round-rect-panel");
			backgroundable->ApplyAlpha(0.5f);

			auto* layoutgroup = container->GetComponent<VerticalLayoutGroup*>();
			RectOffset* offset = RectOffset::New_ctor(2, 2, 0, 0);
			layoutgroup->set_padding(offset);

			CreateTextLayout(container->get_transform(), NoticeBoardText::get_text(), boardLayout);
			// ToggleVisibility();
		}
	}

	PinkCore::UI::NoticeBoard* NoticeBoard::get_instance()
	{
		if (!instance)
		{
			instance = BSML::Helpers::CreateViewController<PinkCore::UI::NoticeBoard*>();
		}
		return instance;
	}

	void NoticeBoard::clear_instance()
	{
		instance = nullptr;
	}

	void NoticeBoard::CreateSwitchingButtons()
	{

		GameObject* canvas = CreateCanvas();
		auto canvas_T = canvas->transform;

		canvas_T->SetParent(transform, true);
		VerticalLayoutGroup* layout = CreateVerticalLayoutGroup(canvas_T);
		HorizontalLayoutGroup* horizon = CreateHorizontalLayoutGroup(layout->get_transform());

		/*
		// Adds a Background to the buttons, but I didn't like the look of it, but kept the code
		auto* backgroundable = horizon->get_gameObject()->GetComponent<Backgroundable*>();
		if (!backgroundable)
		{
			backgroundable = horizon->get_gameObject()->AddComponent<Backgroundable*>();
		}
		backgroundable->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("round-rect-panel"), 0.5f);
		RectOffset* offset = RectOffset::New_ctor(2, 2, 2, 2);
		horizon->set_padding(offset);
		*/

		canvas_T->set_localPosition(Vector3::get_zero());
		canvas_T->GetComponent<RectTransform*>()->set_anchoredPosition({-30.0f, 45.0f});

		layout->set_childControlHeight(true);
		layout->set_childForceExpandHeight(true);
		layout->set_childControlWidth(true);
		layout->set_childForceExpandWidth(true);
		horizon->set_childControlWidth(true);
		horizon->set_childForceExpandWidth(true);

		canvas_T->set_localScale({0.25f, 0.25f, 0.25f});

		noticeBoardButton = CreateUIButton(horizon, "", "SettingsButton", [&](){
			if (state == BoardState::Board) return;
			state = BoardState::Board;
			title->text = titles[state];

			if (donationLayout) donationLayout->SetActive(false);
			if (boardLayout) boardLayout->SetActive(true);
			else CreateTextLayout(container->get_transform(), NoticeBoardText::get_text(), boardLayout);
		});

		UIUtils::SwapButtonSprites(noticeBoardButton, ArrayToSprite(Assets::NewsBoard::NewsIcon_png), ArrayToSprite(Assets::NewsBoard::NewsIconActive_png));

		donationButton = CreateUIButton(horizon, "", "SettingsButton", [&](){
			if (state == BoardState::Donation) return;
			state = BoardState::Donation;
			title->text = titles[state];

			if (boardLayout) boardLayout->SetActive(false);
			if (donationLayout) donationLayout->SetActive(true);
			else CreateTextLayout(container->get_transform(), DonationText::get_text(), donationLayout);
		});

		UIUtils::SwapButtonSprites(donationButton, ArrayToSprite(Assets::NewsBoard::Donation_png), ArrayToSprite(Assets::NewsBoard::DonationActive_png));

		LayoutElement* layoutelem = horizon->get_gameObject()->AddComponent<LayoutElement*>();
		layoutelem->set_preferredHeight(30.0f);
		layoutelem->set_preferredWidth(85.0f);
	}

	void NoticeBoard::CreateTextLayout(Transform* parent, const std::string& text, GameObject*& output)
	{

		HorizontalLayoutGroup* horizon = CreateHorizontalLayoutGroup(parent);
		VerticalLayoutGroup* layout = CreateVerticalLayoutGroup(horizon->get_transform());

		TextMeshProUGUI* tmproText = CreateText(layout->get_transform(), text, false);
		tmproText->set_fontSize(tmproText->get_fontSize() * 0.65f);
		tmproText->set_alignment(TextAlignmentOptions::MidlineLeft);
		tmproText->set_enableWordWrapping(true);

		LayoutElement* noticelayout = tmproText->get_gameObject()->AddComponent<LayoutElement*>();
		layout->set_childControlHeight(true);
		layout->set_childForceExpandHeight(true);
		layout->set_childControlWidth(true);
		layout->set_childForceExpandWidth(true);
		horizon->set_childControlWidth(true);
		horizon->set_childForceExpandWidth(true);
		noticelayout->set_preferredWidth(85.f);

		output = horizon->get_gameObject();
	}

	void NoticeBoard::ToggleVisibility()
	{
		if(container == nullptr) return;
		bool activity = !container->get_active();
		container->SetActive(activity);
		title->get_transform()->get_parent()->get_gameObject()->SetActive(activity);
		noticeBoardButton->get_gameObject()->SetActive(activity);
		donationButton->get_gameObject()->SetActive(activity);

	}

}
