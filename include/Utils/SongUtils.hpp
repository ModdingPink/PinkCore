#pragma once

#include <optional>
#include <string>
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"

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
		void HandleGetMapInfoData(GlobalNamespace::IPreviewBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);

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

		const char16_t* MapEnvironmentTypeChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		int MapSaberCountChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		bool MapHasColoursChecker(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		bool MapShouldShowRotationSpawnLines(rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
		
		GlobalNamespace::ColorScheme* GetCustomSongColourFromCustomData(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData);
		
		GlobalNamespace::ColorScheme* GetCustomSongColour(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride);
		
		void GetCustomCharacteristicItems(StringW characteristic, UnityEngine::Sprite*& sprite, StringW& hoverText);
	

	}

	namespace SongInfo
	{

		
		void ResetMapData();
		void UpdateMapData(CustomJSONData::DocumentUTF16& currentInfoDat, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);


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
		bool get_mapIsCustom();

		void set_mapIsCustom(bool val);
		/*--------------------------------------------------------------*/

		/// @brief getter for whether the current song is WIP
		/// @return true for WIP
		bool get_mapIsWIP();

		void set_mapIsWIP(bool val);
		/*--------------------------------------------------------------*/	

		/// @brief getter for whether the current song has custom colours
		/// @return true for if the level has custom colours
		bool get_mapHasColours();

		void set_mapHasColours(bool val);
		/*--------------------------------------------------------------*/		
		
		/// @brief getter for whether the current song is an overridden OneSaber Map
		/// @return true for OneSaber
		int get_mapSaberCount();

		void set_mapSaberCount(int val);
		/*--------------------------------------------------------------*/

		/// @brief getter for whether the current song shows rotation lines when rotation events are present
		/// @return true for Rotation
		bool get_mapShowsRotationLines();

		void set_mapShowsRotationLines(bool val);
		/*--------------------------------------------------------------*/

		/// @brief getter for the current environment Type string (used to override 360 maps)
		/// @return environment type string
		const char16_t* get_mapEnvironmentTypeString();

		void set_mapEnvironmentTypeString(const char16_t* val);
		/*--------------------------------------------------------------*/

		/// @brief gets the last selected difficulty
		GlobalNamespace::BeatmapDifficulty get_mapDifficulty();
		/*--------------------------------------------------------------*/

		/// @brief gets the latest cached characteristic
		GlobalNamespace::BeatmapCharacteristicSO* get_mapCharacteristic();
		/*--------------------------------------------------------------*/
	}
}