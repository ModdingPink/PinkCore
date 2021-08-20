#pragma once

#include <optional>
#include <string>
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/ColorScheme.hpp"

namespace SongUtils
{
	using LoadedInfoEvent = UnorderedEventCallback<std::optional<std::shared_ptr<rapidjson::Document>>>;
	
	/// @brief gets the event for info loading
	/// @return event callback reference
	LoadedInfoEvent& onLoadedInfo();

	/// @brief gets a reference to the currentl internally loaded info dat
	/// @return rapidjson Document reference of current loaded info dat
	rapidjson::Document& GetCurrentInfoDat();
	
	/// @brief gets a reference to the currentl internally loaded info dat
	/// @return rapidjson Document reference of current loaded info dat
	std::shared_ptr<rapidjson::Document>& GetCurrentInfoDatPtr();

	/// @brief returns the current song path
	const std::string& GetCurrentSongPath();
	
	/// @brief returns string name for specific difficulty
	/// @param selectedDifficulty enum selected
	std::string GetDiffFromNumber(GlobalNamespace::BeatmapDifficulty selectedDifficulty);

	namespace CustomData
	{
		/// @brief gets the info json and loads it into a rapidjson doc if available
		/// @param level the preview level
		/// @param doc reference to the output variable
		/// @return bool success finding file
		//bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, rapidjson::Document& doc);

		/// @brief gets the info json and loads it into a rapidjson doc if available
		/// @param level the preview level
		/// @param doc reference to the output variable
		/// @return bool success finding file
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::Document>& doc);

		/// @brief gets the current custom data from the passed document, and puts it in the passed output
		/// @param in this needs to be a valid info.dat
		/// @param out the value which will be output to
		/// @return bool custom data found and assigned, true for success
		bool GetCurrentCustomData(rapidjson::Document& in, rapidjson::Value& out);		
		
		/// @brief gets the current custom data from the passed document, and puts it in the passed output
		/// @param in this needs to be a valid info.dat
		/// @param out the value which will be output to
		/// @return bool custom data found and assigned, true for success
		bool GetCurrentCustomData(rapidjson::Document& in, rapidjson::Value& out, GlobalNamespace::BeatmapDifficulty difficulty);

		/// @brief using the requirements array, this will extract the requirements for the current array, 
		/// @param requirementsArray the array of requirements
		/// @param output the output to put the requirements into
		void ExtractRequirements(const rapidjson::Value& requirementsArray, std::vector<std::string>& output);

		GlobalNamespace::ColorScheme* getCustomSongColour(GlobalNamespace::ColorScheme* defaultColorScheme, bool hasOverride);

	}

	namespace SongInfo
	{
		/// @brief checks if the levelid is custom
		/// @return true for custom, false for not custom
		bool isCustom(GlobalNamespace::IPreviewBeatmapLevel* level);
		/*--------------------------------------------------------------*/

		/// @brief getter for whether the current song is custom
		/// @return true for custom
		bool get_currentlySelectedIsCustom();

		/// @brief setter for whether the current song is custom
		/// @param value what to set
		void set_currentlySelectedIsCustom(bool value);
		/*--------------------------------------------------------------*/

		/// @brief getter for whether the current song is WIP
		/// @return true for WIP
		bool get_currentlySelectedIsWIP();

		/// @brief setter for whether the current song is WIP
		void set_currentlySelectedIsWIP(bool val);
		/*--------------------------------------------------------------*/

		/// @brief gets if the current info dat is valid
		/// @return true valid, false invalid
		bool get_currentInfoDatValid();

		/// @brief sets if the current info dat is valid
		/// @param value what to set
		void set_currentInfoDatValid(bool value);
		/*--------------------------------------------------------------*/

		/// @brief gets the last selected characteristic string
		/// @return const string reference
		const std::string& get_lastPhysicallySelectedCharacteristic();

		/// @brief sets the last selected characteristic string
		/// @param value what to set
		void set_lastPhysicallySelectedCharacteristic(std::string value);
		/*--------------------------------------------------------------*/

		/// @brief gets the last selected Difficulty string
		/// @return const string reference
		const std::string& get_lastPhysicallySelectedDifficulty();

		/// @brief sets the last selected Difficulty string
		/// @param value what to set
		void set_lastPhysicallySelectedDifficulty(std::string value);
		/*--------------------------------------------------------------*/
	}
}