#pragma once

#include <optional>
#include <string>
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "LevelDetailAPI.hpp"

namespace SongUtils
{

	using LoadedInfoEvent = UnorderedEventCallback<std::optional<std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>>>;

	/// @brief gets the event for info loading
	/// @return event callback reference
	LoadedInfoEvent& onLoadedInfo();

	/// @brief gets a reference to the currentl internally loaded info dat
	/// @return rapidjson Document reference of current loaded info dat
	rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& GetCurrentInfoDat();
	
	/// @brief gets a reference to the currentl internally loaded info dat
	/// @return rapidjson Document reference of current loaded info dat
	std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& GetCurrentInfoDatPtr();

	/// @brief returns the current song path
	const std::u16string& GetCurrentSongPath();
	
	/// @brief returns string name for specific difficulty
	/// @param selectedDifficulty enum selected
	std::u16string GetDiffFromEnum(GlobalNamespace::BeatmapDifficulty selectedDifficulty);

	/// @brief returns BeatmapDifficulty for specific difficulty
	/// @param difficulty string selected
	/// @return returns the difficulty, defaults ExpertPlus
	GlobalNamespace::BeatmapDifficulty GetEnumFromDiff(std::u16string difficulty);

	namespace CustomData
	{

		/// @brief Takes in the current level, difficulty and characteristic and handles the internal custom data
		/// @param level the preview level
		/// @param difficulty the current maps difficulty
		/// @param characteristic the current maps characteristic
		void HandleGetMapInfoData(GlobalNamespace::IPreviewBeatmapLevel* level);

		/// @brief gets the info json and loads it into a rapidjson doc if available
		/// @param level the preview level
		/// @param doc reference to the output variable
		/// @return bool success finding file
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& doc);

		/// @brief gets the current custom data from the passed document, and puts it in the passed output
		/// @param in this needs to be a valid info.dat
		/// @param out the value which will be output to
		/// @return bool custom data found and assigned, true for success
		bool GetCurrentCustomDataJson(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& out);		
		
		/// @brief gets the current custom data from the passed document, and puts it in the passed output
		/// @param in this needs to be a valid info.dat
		/// @param out the value which will be output to
		/// @param difficulty current maps difficulty
		/// @param characteristic current maps characteristic
		/// @return bool custom data found and assigned, true for success
		bool GetCustomDataJsonFromDifficultyAndCharacteristic(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& out, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		/// @brief gets the current custom data from the passed document, and puts it in the passed output
		/// @param in this needs to be a valid info.dat
		/// @param out the value which will be output to
		/// @param characteristic current maps characteristic
		/// @return bool custom data found and assigned, true for success
		bool GetCustomDataJsonFromCharacteristic(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& out, GlobalNamespace::BeatmapCharacteristicSO* characteristic);

		/// @brief using the requirements array, this will extract the requirements for the current array
		/// @param requirementsArray the array of requirements
		/// @param output the output to put the requirements into
		void ExtractRequirements(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& requirementsArray, std::vector<std::string>& output);

		/// @brief gets if the current info dat is valid
		/// @return true valid, false invalid
		bool get_currentInfoDatValid();

		/// @brief sets if the current info dat is valid
		/// @param value what to set
		void set_currentInfoDatValid(bool value);


		GlobalNamespace::ColorScheme* GetCustomSongColourFromCustomData(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData);

		GlobalNamespace::ColorScheme* GetCustomSongColour(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride);
		
		void GetCustomCharacteristicItems(GlobalNamespace::BeatmapCharacteristicSO* characteristic, UnityEngine::Sprite*& sprite, StringW& hoverText);

		int MapSaberCountChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		bool MapHasColoursChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		bool MapShouldShowRotationSpawnLines(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		const char16_t* MapEnvironmentTypeChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		

	}

	namespace SongInfo
	{

		
		void ResetMapData();
		void UpdateMapData(GlobalNamespace::IPreviewBeatmapLevel* level, rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);

		/// @brief checks if the levelid is custom
		/// @return true for custom, false for not custom
		bool isCustom(GlobalNamespace::IPreviewBeatmapLevel* level);
		/*--------------------------------------------------------------*/

		/// @brief checks if the levelid is WIP
		/// @return true for WIP, false for not WIP
		bool isWIP(GlobalNamespace::IPreviewBeatmapLevel* level);
		/*--------------------------------------------------------------*/

		/// @brief getter for whether the current song is custom
		/// @return true for custom
		PinkCore::API::LevelDetails get_mapData();

		void set_mapIsCustom(bool val);
		/*--------------------------------------------------------------*/

		void set_mapIsWIP(bool val);
		/*--------------------------------------------------------------*/	

	}
}