#pragma once

#include "UnityEngine/Color.hpp"
#include "UnityEngine/Vector2.hpp"
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/LevelListTableCell.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "HMUI/ViewController.hpp"
#include "HMUI/TitleViewController.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Sprite.hpp"
#include "UnityEngine/UI/Button.hpp"
#include <string>


namespace UIUtils
{
	/// @brief adds a header to the given parent with text title
	/// @param parent the parent of the title
	/// @param title the text to display
	/// @return the text object that is in the header
	TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title);

	/// @brief adds a header to the given parent with text title
	/// @param parent the parent of the title
	/// @param title the text to display
	/// @param color the color to use for the fade, goes from full color to full transparent
	/// @return the text object that is in the header
	TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Color color);

	/// @brief adds a header to the given parent with text title
	/// @param parent the parent of the title
	/// @param title the text to display
	/// @param leftColor the color on the left of the header
	/// @param rightColor the color on the right of the header
	/// @return the text object that is in the header
	TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Color leftColor, UnityEngine::Color rightColor);

	/// @brief adds a header to the given parent with text title
	/// @param parent the parent of the title
	/// @param title the text to display
	/// @param size the size of the header
	/// @param leftColor the color on the left of the header
	/// @param rightColor the color on the right of the header
	/// @return the text object that is in the header
	TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Vector2 size, UnityEngine::Color leftColor, UnityEngine::Color rightColor);

	/// @brief adds a header to the given parent with text title
	/// @param parent the parent of the title
	/// @param title the text to display
	/// @param size the size of the header
	/// @param anchoredPosition the position of the header
	/// @param leftColor the color on the left of the header
	/// @param rightColor the color on the right of the header
	/// @return the text object that is in the header
	TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, std::string title, UnityEngine::Vector2 size, UnityEngine::Vector2 anchoredPosition, UnityEngine::Color leftColor, UnityEngine::Color rightColor);

	/// @brief sets the color for a given title view, can turn off the button animation if needed
	/// @param titleView the titleview to set the color for
	/// @param color the color to set
	/// @param buttonanim whether the button animation should work (this prevents colors from being weird)
	void SetTitleColor(HMUI::TitleViewController* titleView, UnityEngine::Color color, bool buttonanim = false);

	/// @brief swaps the button sprites for a given button
	/// @param button the button to swap sprites for
	/// @param normalName the path to the image for the button being inactive
	/// @param selectedName the path to the image for button highlighted
	void SwapButtonSprites(UnityEngine::UI::Button* button, std::string normalName, std::string selectedName);

	void SwapButtonSprites(UnityEngine::UI::Button* button, UnityEngine::Sprite* normal, UnityEngine::Sprite* highlighted);

	/// @brief sets up or updates the requirements modal
	void SetupOrUpdateRequirementsModal(GlobalNamespace::StandardLevelDetailView* self);

	/// @brief loads sprite at given path
	UnityEngine::Sprite* FileToSprite(std::string_view path);

	/// @brief loads sprite at given u16 path
	UnityEngine::Sprite* FileToSprite(std::u16string_view path); 

	//static auto WIPLevelString = ConstString("WIP Level");
    //static auto WorkInProgressString = ConstString("Please Play in Practice Mode");
    //static auto colourAvailableString = ConstString("Custom Colours Available");
    //static auto previewColourString = ConstString("Click here to preview & enable them.");
}