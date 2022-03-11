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
//#include "CustomTypes/RequirementHandler.hpp"
//#include "CustomTypes/ContributorHandler.hpp"
#include "CustomTypes/RequirementModalListTableData.hpp"

#include "Utils/RequirementUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/SongUtils.hpp"

#include "questui/shared/ArrayUtil.hpp"
#include "System/Action.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

#include "static-defines.h"
#include <fstream>
#include "logging.hpp"

#include "assets.hpp"

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
	HMUI::ModalView* modal;

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

	
	void SetupOrUpdateRequirementsModal(GlobalNamespace::StandardLevelDetailView* self)
	{
		auto requirementsList = self->get_gameObject()->GetComponentInChildren<PinkCore::UI::RequirementModalListTableData*>(true);
		//auto requirementsHandler = self->GetComponentInChildren<PinkCore::UI::RequirementHandler*>(true);
		//auto contributorsHandler = self->GetComponentInChildren<PinkCore::UI::ContributorHandler*>(true);

		bool isNew = false;

		// if anything is needed, anyone worked on it, or the song is WIP, show the modal
		bool showModal = (RequirementUtils::IsAnythingNeeded() || ContributorUtils::DidAnyoneWorkOnThis() || SongUtils::SongInfo::get_currentlySelectedIsWIP());
		
		if (!requirementsList && showModal)
		{
			isNew = true;
			INFO("requirementsList did not exist, making it...");
			using namespace UnityEngine;
			using namespace UnityEngine::UI;

			Vector2 pos = Vector2(30.5f, 25.0f);
			VerticalLayoutGroup* layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(self->get_transform());
			HorizontalLayoutGroup* horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(layout->get_transform());


			

			UnityEngine::UI::Button* button = QuestUI::BeatSaberUI::CreateUIButton(horizon->get_transform(), "", "PlayButton", Vector2(0.0f, 0.0f), Vector2(15.0f, 12.0f), [&](){
				modal->Show(true, false, nullptr);
			});
			
			auto text = QuestUI::BeatSaberUI::CreateText(button->get_transform(), "?");
			text->set_alignment(TMPro::TextAlignmentOptions::Center);

			layout->get_transform()->set_localScale(Vector3::get_one() * 0.5f);
			layout->GetComponent<RectTransform*>()->set_anchoredPosition(pos);

			

			modal = QuestUI::BeatSaberUI::CreateModal(button->get_transform(), UnityEngine::Vector2(55.0f, 70.0f), UnityEngine::Vector2(-15.0f, 0.0f), nullptr);
			//UnityEngine::GameObject* container = QuestUI::BeatSaberUI::CreateScrollableModalContainer(modal);

			

			/*horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_transform());
			layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizon->get_transform());*/
			
			
			//requirementsHandler = container->get_gameObject()->AddComponent<PinkCore::UI::RequirementHandler*>();
			requirementsList = CreateScrollableCustomSourceList<PinkCore::UI::RequirementModalListTableData*>(modal->get_transform(), Vector2(0.0f, -35.0f), Vector2(55.0f, 70.0f), nullptr);
			/*horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_transform());
			layout = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(horizon->get_transform());*/
			
			//contributorsHandler = container->get_gameObject()->AddComponent<PinkCore::UI::ContributorHandler*>();
		
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
				modal = modalView;
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
