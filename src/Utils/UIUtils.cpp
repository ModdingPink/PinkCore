// file copied from https://github.com/RedBrumbler/Qosmetics/blob/rewrite/src/Utils/UIUtils.cpp
#include "Utils/UIUtils.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/CanvasGroup.hpp"
#include "UnityEngine/CanvasRenderer.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/ImageConversion.hpp"
#include "UnityEngine/SpriteMeshType.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Texture.hpp"
#include "UnityEngine/TextureWrapMode.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/AdditionalCanvasShaderChannels.hpp"
#include "UnityEngine/RenderMode.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/UI/LayoutRebuilder.hpp"

#include "HMUI/TitleViewController.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/ButtonStaticAnimations.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "HMUI/ButtonSpriteSwap.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/AnnotatedBeatmapLevelCollectionsViewController.hpp"
#include "GlobalNamespace/ColorSchemeView.hpp"
//#include "CustomTypes/RequirementHandler.hpp"
//#include "CustomTypes/ContributorHandler.hpp"
#include "CustomTypes/RequirementModalListTableData.hpp"

#include "Utils/RequirementUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/SongUtils.hpp"

#include "questui/shared/ArrayUtil.hpp"
#include "System/Action.hpp"
#include "custom-types/shared/delegate.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "static-defines.h"
#include <fstream>
#include "logging.hpp"

#include "assets.hpp"
#include "config.hpp"

using namespace VRUIControls;
using namespace HMUI;
using namespace UnityEngine;
using namespace UnityEngine::Events;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace TMPro;


namespace UIUtils
{
	HMUI::ModalView* requirementsModal;
	HMUI::ModalView* coloursModal;
	GlobalNamespace::ColorSchemeView* colourSchemeView;

	TextMeshProUGUI* AddHeader(Transform* parent, std::string title)
	{
		Color color = {0.0f, 0.75f, 1.0f, 1.0f};
		return AddHeader(parent, title, color);
	}

	TextMeshProUGUI* AddHeader(Transform* parent, std::string title, Color color)
	{
		Color otherColor = color;
		otherColor.a = 0.0f;
		return AddHeader(parent, title, color, otherColor);
	}

	TextMeshProUGUI* AddHeader(Transform* parent, std::string title, Color leftColor, Color rightColor)
	{
		return AddHeader(parent, title, {90.0f, 10.0f}, leftColor, rightColor);
	}

	TextMeshProUGUI* AddHeader(Transform* parent, std::string title, Vector2 size, Color leftColor, Color rightColor)
	{
		return AddHeader(parent, title, size, {0, 45.0f}, leftColor, rightColor);
	}

	TextMeshProUGUI* AddHeader(Transform* parent, std::string title, Vector2 size, Vector2 anchoredPosition, Color leftColor, Color rightColor)
	{
		VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(parent);
		vertical->get_rectTransform()->set_anchoredPosition(anchoredPosition);
		HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(vertical->get_transform());

		TextMeshProUGUI* text = CreateText(horizontal->get_transform(), title, false);
		text->set_fontSize(text->get_fontSize() * 2.0f);
		text->set_alignment(TextAlignmentOptions::Center);

		LayoutElement* layoutelem = text->get_gameObject()->AddComponent<LayoutElement*>();
		layoutelem->set_preferredHeight(size.y);
		layoutelem->set_preferredWidth(size.x);

		Backgroundable* background = horizontal->get_gameObject()->AddComponent<Backgroundable*>();
		background->ApplyBackgroundWithAlpha(il2cpp_utils::newcsstr("title-gradient"), 1.0f);

		ImageView* imageView = background->get_gameObject()->GetComponentInChildren<ImageView*>();
		imageView->gradient = true;
		imageView->gradientDirection = 0;
		imageView->set_color(Color::get_white());
		imageView->set_color0(leftColor);
		imageView->set_color1(rightColor);
		imageView->skew = 0.18f;
		imageView->curvedCanvasSettingsHelper->Reset();
		return text;
	}

	void SetTitleColor(HMUI::TitleViewController* titleView, UnityEngine::Color color, bool buttonanim)
	{
		if (!titleView)
		{
			ERROR("Title View was nullptr, not setting title color");
			return;
		}

		Transform* title_T = titleView->get_transform();
		Transform* BG_T = title_T->Find(il2cpp_utils::newcsstr("BG"));
		Transform* BackButtonBG_T = title_T->Find(il2cpp_utils::newcsstr("BackButton/BG"));

		ImageView* imageView = BG_T->get_gameObject()->GetComponent<ImageView*>();
		Color oldColor = imageView->get_color();

		imageView->set_color(color);

		ImageView* buttonImageView = BackButtonBG_T->get_gameObject()->GetComponent<ImageView*>();
		buttonImageView->set_color(color);
		buttonImageView->set_color0(color);
		buttonImageView->set_color1(color * 0.9f);

		ButtonStaticAnimations* anim = BackButtonBG_T->get_parent()->get_gameObject()->GetComponent<ButtonStaticAnimations*>();
		anim->set_enabled(buttonanim);
	}

	void SwapButtonSprites(UnityEngine::UI::Button* button, UnityEngine::Sprite* normal, UnityEngine::Sprite* highlighted)
	{
		Texture* highl = highlighted->get_texture();
		highl->set_wrapMode(UnityEngine::TextureWrapMode::_get_Clamp());
		UnityEngine::Sprite* pressed = highlighted;
		UnityEngine::Sprite* selected = normal;
		Texture* sel = selected->get_texture();
		sel->set_wrapMode(UnityEngine::TextureWrapMode::_get_Clamp());
		UnityEngine::Sprite* disabled = selected;
		HMUI::ButtonSpriteSwap* spriteSwap = button->get_gameObject()->GetComponent<HMUI::ButtonSpriteSwap*>();
		spriteSwap->normalStateSprite = selected;
		spriteSwap->highlightStateSprite = highlighted;
		spriteSwap->pressedStateSprite = pressed;
		spriteSwap->disabledStateSprite = disabled;

		auto imageView = button->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
		if (imageView) imageView->skew = 0.18f; 
	}

	void SwapButtonSprites(UnityEngine::UI::Button* button, std::string normalName, std::string selectedName)
	{
		UnityEngine::Sprite* highlighted = UIUtils::FileToSprite(selectedName);
		Texture* highl = highlighted->get_texture();
		highl->set_wrapMode(UnityEngine::TextureWrapMode::_get_Clamp());
		UnityEngine::Sprite* pressed = highlighted;
		UnityEngine::Sprite* selected = UIUtils::FileToSprite(normalName);
		Texture* sel = selected->get_texture();
		sel->set_wrapMode(UnityEngine::TextureWrapMode::_get_Clamp());
		UnityEngine::Sprite* disabled = selected;
		HMUI::ButtonSpriteSwap* spriteSwap = button->get_gameObject()->GetComponent<HMUI::ButtonSpriteSwap*>();
		spriteSwap->normalStateSprite = selected;
		spriteSwap->highlightStateSprite = highlighted;
		spriteSwap->pressedStateSprite = pressed;
		spriteSwap->disabledStateSprite = disabled;

		auto imageView = button->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
		if (imageView) imageView->skew = 0.18f; 
	}


	void SetPlaylistViewState(bool state){
		auto levelPackView = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::AnnotatedBeatmapLevelCollectionsViewController*>().LastOrDefault();
		if (levelPackView) levelPackView->get_transform()->get_parent()->get_gameObject()->SetActive(state);
	}

	static ConstString contentName("Content");

	void SetupOrShowColorsModal(Transform* parent)
	{
		static ConstString modalName("PinkCoreColoursModal");
		auto coloursModalTranform = parent->Find(modalName);

		if (!coloursModalTranform) {
			INFO("Colours modal did not exist, making it");
			coloursModal = CreateModal(parent, {85, 35}, {-7.5, 8}, nullptr);
			coloursModal->set_name(modalName);

			auto layout = CreateVerticalLayoutGroup(coloursModal);
			layout->GetComponent<LayoutElement*>()->set_preferredWidth(80);

			auto colourToggle = CreateToggle(layout, "Use Custom Song Colors", config.enableCustomSongColours, [](bool enabled) {
				config.enableCustomSongColours = enabled;
				SaveConfig();
			});
			
			AddHoverHint(colourToggle, "Allow Custom Songs to override note/light colors");
			
			auto playerNoteToggle = CreateToggle(layout, "Ignore Custom Song Note Colors", config.forceNoteColours, [](bool enabled) {
				config.forceNoteColours = enabled;
				SaveConfig();
			});
			AddHoverHint(playerNoteToggle, "Force your own note colours, selected from the left 'Colors' menu");

			auto horizontal = CreateHorizontalLayoutGroup(layout);
			horizontal->set_spacing(-24);

			CreateText(horizontal, "Selected Custom Song's Colors");

			colourSchemeView = Object::Instantiate(Resources::FindObjectsOfTypeAll<GlobalNamespace::ColorSchemeView*>().Last(), horizontal->get_transform(), false);

			// doesn't work? basically copied from songcore though
			AddHoverHint(colourSchemeView->saberAColorImage, "Left Saber Color");
			AddHoverHint(colourSchemeView->saberBColorImage, "Right Saber Color");
			AddHoverHint(colourSchemeView->environment0ColorImage, "Primary Light Color");
			AddHoverHint(colourSchemeView->environment1ColorImage, "Secondary Light Color");
			AddHoverHint(colourSchemeView->obstacleColorImage, "Wall Color");
			AddHoverHint(colourSchemeView->environmentColor0BoostImage, "Primary Light Boost Color");
			AddHoverHint(colourSchemeView->environmentColor1BoostImage, "Secondary Light Boost Color");



		}

		coloursModal->get_gameObject()->GetComponentInChildren<Toggle*>()->set_isOn(config.enableCustomSongColours);

		static auto colorSchemeId = (StringW) ConstString("PinkCoreVoidColorScheme");
		static auto colorSchemeNameLocalizationKey = (StringW) ConstString("PinkCore Void Color Scheme");
		static Color voidColour(0.5, 0.5, 0.5, 0.25);
		static auto voidColourScheme = *il2cpp_utils::New<GlobalNamespace::ColorScheme*, il2cpp_utils::CreationType::Manual>(colorSchemeId, colorSchemeNameLocalizationKey, true, colorSchemeNameLocalizationKey, false,
			voidColour, voidColour, voidColour, voidColour, false, voidColour, voidColour, voidColour);

		if (auto scheme = SongUtils::CustomData::GetCustomSongColour(voidColourScheme, false))
		{
			reinterpret_cast<UnityEngine::RectTransform*>(coloursModal->get_transform())->set_anchoredPosition(UnityEngine::Vector2(-7.5, 8));
			coloursModal->Show(true, false, nullptr);

			colourSchemeView->SetColors(scheme->saberAColor, scheme->saberBColor, scheme->environmentColor0, scheme->environmentColor1, scheme->environmentColor0Boost, scheme->environmentColor1Boost, scheme->obstaclesColor);
		}
	}

	void SetupOrUpdateRequirementsModal(GlobalNamespace::StandardLevelDetailView* self)
	{
		auto requirementsList = self->get_gameObject()->GetComponentInChildren<PinkCore::UI::RequirementModalListTableData*>(true);
		//auto requirementsHandler = self->GetComponentInChildren<PinkCore::UI::RequirementHandler*>(true);
		//auto contributorsHandler = self->GetComponentInChildren<PinkCore::UI::ContributorHandler*>(true);

		bool isNew = false;

		// if anything is needed, anyone worked on it, or the song is WIP, show the modal
		bool showModal = (RequirementUtils::IsAnythingNeeded() || ContributorUtils::DidAnyoneWorkOnThis() || SongUtils::SongInfo::get_currentlySelectedIsWIP() || SongUtils::SongInfo::get_currentlySelectedHasColours());
		
		auto levelViews = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailView*>();
		int length = levelViews.Length();
		UnityEngine::RectTransform* heartRectTrans; 
		if (length > 0)
		{
			heartRectTrans = reinterpret_cast<UnityEngine::RectTransform*>(levelViews[length - 1]->favoriteToggle->get_transform());
			if(showModal) heartRectTrans->set_anchoredPosition({3, -2});
			else heartRectTrans->set_anchoredPosition({3, 0});
		}

		if (!requirementsList && showModal)
		{
			isNew = true;
			INFO("requirementsList did not exist, making it...");
			using namespace UnityEngine;
			using namespace UnityEngine::UI;

			Vector2 pos = Vector2(30.5f, 25.0f);
			VerticalLayoutGroup* layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());
			HorizontalLayoutGroup* horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(layout->get_transform());

			auto button = BeatSaberUI::CreateUIButton(horizon->get_transform(), "?", "PlayButton", {0, 0}, {9.0f, 7.2f}, [] {
				reinterpret_cast<UnityEngine::RectTransform*>(requirementsModal->get_transform())->set_anchoredPosition(UnityEngine::Vector2(-27.0f, -15.0f));
				requirementsModal->Show(true, false, nullptr);
			});
			//haha YOINK, THANKS FOR THE BUTTON CODE METALIT WOOOOOOOOOOOOOOOOOO, WE LOVE GPL-3.0
			auto text = button->GetComponentInChildren<TMPro::TextMeshProUGUI*>();
			UnityEngine::Object::Destroy(button->get_transform()->Find(contentName)->GetComponent<UnityEngine::UI::LayoutElement*>());
			auto sizeFitter = button->get_gameObject()->AddComponent<UnityEngine::UI::ContentSizeFitter*>();
			sizeFitter->set_horizontalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
			sizeFitter->set_verticalFit(UnityEngine::UI::ContentSizeFitter::FitMode::PreferredSize);
			text->set_fontSize(3);

			layout->GetComponent<RectTransform*>()->set_anchoredPosition(pos);

			requirementsModal = QuestUI::BeatSaberUI::CreateModal(button->get_transform(), UnityEngine::Vector2(58.0f, 65.0f), UnityEngine::Vector2(0.0f, 0.0f), nullptr);
		
			requirementsList = CreateScrollableCustomSourceList<PinkCore::UI::RequirementModalListTableData*>(requirementsModal->get_transform(), Vector2(0.0f, -32.25f), Vector2(55.0f, 63.5f), [self](int cell) {
				// can't capture requirementsList when it hasn't been set yet, and this is better than making it a global variable imo
				requirementsModal->get_gameObject()->GetComponentInChildren<PinkCore::UI::RequirementModalListTableData*>()->tableView->ClearSelection();
				// check if color cell
				if (cell == 0 && SongUtils::SongInfo::get_currentlySelectedHasColours()) {
					// modals seem to be buggy when stacked
					requirementsModal->Hide(true, custom_types::MakeDelegate<System::Action*>((std::function<void()>) [self] {
						SetupOrShowColorsModal(self->get_transform());
					}));
				}
			});
			//
		
		}
		else if (requirementsList)
		{
			UnityEngine::Transform* parent = requirementsList->get_transform();
			UnityEngine::UI::Button* button = nullptr;

			// GetComponentInParent doesnt work on disabled objects, so lets do it manually 
			while (parent)
			{
				button = parent->GetComponent<UnityEngine::UI::Button*>();
				if (button) break;
				parent = parent->get_parent();
			}

			if (button)
				button->get_gameObject()->SetActive(showModal);

			parent = requirementsList->get_transform();

			// update modal value just to be sure
			HMUI::ModalView* modalView;
			while (parent)
			{
				modalView = parent->GetComponent<HMUI::ModalView*>();
				if (modalView) break;
				parent = parent->get_parent();
			}
			if (modalView)
				requirementsModal = modalView;
			

		}

		if (requirementsList) 
			requirementsList->Refresh();
		//RequirementUtils::UpdateRequirementHandler(requirementsHandler, isNew);
		//ContributorUtils::UpdateContributorHandler(contributorsHandler, isNew);
	}
	
	UnityEngine::Sprite* FileToSprite(std::u16string_view path) 
	{
		INFO("FileToSprite Path: %s", to_utf8(path).c_str());
		if (!fileexists(to_utf8(path))) return VectorToSprite(std::vector<uint8_t>(_binary_MissingSprite_png_start, _binary_MissingSprite_png_end));
		std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<uint8_t> data = std::vector<uint8_t>(std::istreambuf_iterator<char>(instream), std::istreambuf_iterator<char>());
		return VectorToSprite(data);
	}

	UnityEngine::Sprite* FileToSprite(std::string_view path)
    {
		if (!fileexists(path)) return VectorToSprite(std::vector<uint8_t>(_binary_MissingSprite_png_start, _binary_MissingSprite_png_end));
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        Array<uint8_t>* bytes = il2cpp_utils::vectorToArray(data);
        UnityEngine::Texture2D* texture = UnityEngine::Texture2D::New_ctor(0, 0, UnityEngine::TextureFormat::RGBA32, false, false);
        if (UnityEngine::ImageConversion::LoadImage(texture, bytes, false)) {
            texture->set_wrapMode(UnityEngine::TextureWrapMode::Clamp);
            return UnityEngine::Sprite::Create(texture, UnityEngine::Rect(0.0f, 0.0f, (float)texture->get_width(), (float)texture->get_height()), UnityEngine::Vector2(0.5f,0.5f), 100.0f, 1u, UnityEngine::SpriteMeshType::FullRect, UnityEngine::Vector4(0.0f, 0.0f, 0.0f, 0.0f), false);
        }
        return VectorToSprite(std::vector<uint8_t>(_binary_MissingSprite_png_start, _binary_MissingSprite_png_end));
    }
}
