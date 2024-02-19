#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/FilteredBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "logging.hpp"
#include "config.hpp"
#include "LevelDetailAPI.hpp"
#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "Utils/UIUtils.hpp"
#include <fstream>
#include "Utils/ContributorUtils.hpp"

static std::string toLower(std::string in)
{
	std::string output = "";
	for (auto& c : in)
	{
		output += tolower(c);
	}

	return output;
}

static std::string removeSpaces(std::string_view input)
{
	std::string output;
    output.reserve(input.size());
	for (auto c : input)
	{
		if (c == ' ') continue;
		output += c;
	}
    output.shrink_to_fit();
	return output;
}

namespace SongUtils
{

	LoadedInfoEvent onLoadedInfoEvent;
	std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>> currentInfoDat;
	std::u16string currentLevelPath = u"";
	bool currentInfoDatValid;

	LoadedInfoEvent& onLoadedInfo()
	{
		return onLoadedInfoEvent;
	}

	std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& GetCurrentInfoDatPtr()
	{
		return currentInfoDat;
	}

	rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& GetCurrentInfoDat()
	{
		return *currentInfoDat;
	}

	const std::u16string& GetCurrentSongPath()
	{
		return currentLevelPath;
	}

	static inline std::optional<std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>> getOptional(bool value)
	{
		if (value) return currentInfoDat;
		else return std::nullopt;
	}

	std::u16string GetDiffFromEnum(GlobalNamespace::BeatmapDifficulty selectedDifficulty)
	{
		switch (selectedDifficulty)
		{
			case GlobalNamespace::BeatmapDifficulty::Easy:
				return u"Easy";
				break;
			case GlobalNamespace::BeatmapDifficulty::Normal:
				return u"Normal";
				break;
			case GlobalNamespace::BeatmapDifficulty::Hard:
				return u"Hard";
				break;
			case GlobalNamespace::BeatmapDifficulty::Expert:
				return u"Expert";
				break;
			case GlobalNamespace::BeatmapDifficulty::ExpertPlus:
				return u"ExpertPlus";
				break;
			default:
				return u"Unknown";
				break;
		}
	}

	GlobalNamespace::BeatmapDifficulty GetEnumFromDiff(std::u16string difficulty)
	{
		if (difficulty == u"Easy") return GlobalNamespace::BeatmapDifficulty::Easy;
		else if (difficulty == u"Normal") return GlobalNamespace::BeatmapDifficulty::Normal;
		else if (difficulty == u"Hard") return GlobalNamespace::BeatmapDifficulty::Hard;
		else if (difficulty == u"Expert") return GlobalNamespace::BeatmapDifficulty::Expert;
		else return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
	}

	namespace CustomData
	{

		bool currentInfoDatValid = false;

		bool get_currentInfoDatValid()
		{
			return currentInfoDatValid;
		}

		void set_currentInfoDatValid(bool value)
		{
			currentInfoDatValid = value;
			onLoadedInfoEvent.invoke(getOptional(value));
		}


		void HandleGetMapInfoData(GlobalNamespace::IPreviewBeatmapLevel* level){
			bool isCustom = false;
			bool isWIP = false;
			if(level) {
				isCustom = SongUtils::SongInfo::isCustom(level);
				isWIP = SongUtils::SongInfo::isWIP(level);
			}
			SongInfo::set_mapIsCustom(isCustom);
			SongInfo::set_mapIsWIP(isWIP);
			if (isCustom)
			{
				// clear current info dat
				auto& d = SongUtils::GetCurrentInfoDatPtr();
				if (SongUtils::CustomData::GetInfoJson(level, d))
				{
					INFO("Info.dat read successful!");
					SongUtils::CustomData::set_currentInfoDatValid(true);
				}
				else
				{
					//custom dat read not successful
					SongInfo::ResetMapData();
					SongUtils::CustomData::set_currentInfoDatValid(false);
					INFO("Info.dat read not successful!");
				}

			}
			else
			{
				//base game map
				SongInfo::ResetMapData();
				SongUtils::CustomData::set_currentInfoDatValid(false);
				INFO("Level was not custom!");
			}
		}


		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomPreviewBeatmapLevel* customLevel;
			if (auto filter = il2cpp_utils::try_cast<GlobalNamespace::FilteredBeatmapLevel>(level)) {
				customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(filter.value()->_beatmapLevel);
			} else {
				customLevel = il2cpp_utils::cast<GlobalNamespace::CustomPreviewBeatmapLevel>(level);
			}
			std::u16string songPath(customLevel->get_customLevelPath());
			currentLevelPath = songPath;

			auto infoDataOpt = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(customLevel->get_standardLevelInfoSaveData());
			// if an info.dat already exists on the given level, don't read the file again
			if (infoDataOpt)
			{
				INFO("Found custom json data on level");
				auto infoData = infoDataOpt.value();
				doc = infoData->doc;
				return true;
			}
			else
			{
				return false;
				/*
				songPath += u"/info.dat";
				// get the path
				INFO("Getting info.dat for %s", to_utf8(songPath).c_str());
				// if the file doesnt exist, fail
				//if (!fileexists(songPath)) return false;

				INFO("Reading file");
				// read file
				std::ifstream instream(songPath, std::ios::in);
				instream.seekg(0, instream.end);
				size_t size = (size_t)instream.tellg();
				size -= 2;
				std::u16string info;
				info.resize(size);
				instream.seekg(2, instream.beg);
				instream.read((char*)&info[0], size);

				// parse into doc
				if (!doc) doc = std::make_shared<typename rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>();
				doc->Parse(info.data());
				// return true if it read the file right, return false if there was a parse error
				return !doc->GetParseError();*/
			}
		}


		bool GetCurrentCustomDataJson(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, CustomJSONData::ValueUTF16& out)
		{
			return GetCustomDataJsonFromDifficultyAndCharacteristic(in, out, SongInfo::get_mapData().difficulty, SongInfo::get_mapData().characteristic);
		}

		bool GetCustomDataJsonFromCharacteristic(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, CustomJSONData::ValueUTF16& out, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{
			bool hasCustomData = false;
			INFO("Looking for characteristic: %s", to_utf8(characteristic->serializedName).c_str());

			auto difficultyBeatmapSetsitr = in.FindMember(u"_difficultyBeatmapSets");
			// if we find the sets iterator
			if (difficultyBeatmapSetsitr != in.MemberEnd())
			{
				auto setArr = difficultyBeatmapSetsitr->value.GetArray();
				for (auto& beatmapCharacteristicItr : setArr)
				{
					std::u16string beatmapCharacteristicName = beatmapCharacteristicItr.FindMember(u"_beatmapCharacteristicName")->value.GetString();
					INFO("Found CharacteristicName: %s", (char*)beatmapCharacteristicName.c_str());
					// if the last selected beatmap characteristic is this specific one
					if (beatmapCharacteristicName == characteristic->serializedName)
					{
						hasCustomData = true;
						//auto difficultyBeatmaps = beatmapCharacteristicItr.GetObject().FindMember(u"_difficultyBeatmaps");
						out.CopyFrom(beatmapCharacteristicItr, in.GetAllocator());
					}
				}
			}
			return hasCustomData;
		}


		bool GetCustomDataJsonFromDifficultyAndCharacteristic(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, CustomJSONData::ValueUTF16& out, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{
			bool hasCustomData = false;
			CustomJSONData::ValueUTF16 customData;
			std::u16string difficultyToFind = SongUtils::GetDiffFromEnum(difficulty);
			if(GetCustomDataJsonFromCharacteristic(in, customData, characteristic)){
				auto difficultyBeatmaps = customData.GetObject().FindMember(u"_difficultyBeatmaps");
				auto beatmaps = difficultyBeatmaps->value.GetArray();
				for (auto& beatmap : beatmaps)
				{
					auto beatmapDiffNameItr = beatmap.GetObject().FindMember(u"_difficulty");
					std::u16string diffString = beatmapDiffNameItr->value.GetString();
					INFO("Found diffstring: %s", (char*)diffString.c_str());
					// if the last selected difficulty is this specific one
					if (difficultyToFind == diffString)
					{
						auto specificCustomData = beatmap.GetObject().FindMember(u"_customData");
						if (specificCustomData != beatmap.MemberEnd())
						{
							hasCustomData = true;
							out.CopyFrom(specificCustomData->value, in.GetAllocator());
						}
					}
				}
			}
			return hasCustomData;

			/*
			bool hasCustomData = false;
			std::u16string difficultyToFind = SongUtils::GetDiffFromEnum(difficulty);
			INFO("Looking for characteristic: %s", to_utf8(characteristic->serializedName).c_str());
			INFO("Looking for diff: %s", to_utf8(difficultyToFind).c_str());

			auto difficultyBeatmapSetsitr = in.FindMember(u"_difficultyBeatmapSets");
			// if we find the sets iterator
			if (difficultyBeatmapSetsitr != in.MemberEnd())
			{
				auto setArr = difficultyBeatmapSetsitr->value.GetArray();
				for (auto& beatmapCharacteristicItr : setArr)
				{
					std::u16string beatmapCharacteristicName = beatmapCharacteristicItr.FindMember(u"_beatmapCharacteristicName")->value.GetString();
					INFO("Found CharacteristicName: %s", (char*)beatmapCharacteristicName.c_str());
					// if the last selected beatmap characteristic is this specific one
					if (beatmapCharacteristicName == characteristic->serializedName)
					{
						auto difficultyBeatmaps = beatmapCharacteristicItr.GetObject().FindMember(u"_difficultyBeatmaps");
						auto beatmaps = difficultyBeatmaps->value.GetArray();
						for (auto& beatmap : beatmaps)
						{
							auto beatmapDiffNameItr = beatmap.GetObject().FindMember(u"_difficulty");
							std::u16string diffString = beatmapDiffNameItr->value.GetString();
							INFO("Found diffstring: %s", (char*)diffString.c_str());
							// if the last selected difficulty is this specific one
							if (difficultyToFind == diffString)
							{
								auto customData = beatmap.GetObject().FindMember(u"_customData");
								if (customData != beatmap.MemberEnd())
								{
									hasCustomData = true;
									out.CopyFrom(customData->value, in.GetAllocator());
								}
							}
						}
					}
				}
			}
			return hasCustomData;*/
		}



		void GetCustomCharacteristicItems(GlobalNamespace::BeatmapCharacteristicSO* characteristic, UnityEngine::Sprite*& sprite, StringW& hoverText){
			auto& d = SongUtils::GetCurrentInfoDat();
			bool hasCustomData = false;
			CustomJSONData::ValueUTF16 customData;
			GetCustomDataJsonFromCharacteristic(d, customData, characteristic);
			auto customDataItr = customData.GetObject().FindMember(u"_customData");
			if(customDataItr != customData.MemberEnd()){
				auto characteristicLabel = customDataItr->value.GetObject().FindMember(u"_characteristicLabel");
				auto characteristicIconFilePath = customDataItr->value.FindMember(u"_characteristicIconImageFilename");
				if(characteristicLabel != customDataItr->value.GetObject().MemberEnd()){
					hoverText = characteristicLabel->value.GetString();
				}
				if(characteristicIconFilePath != customDataItr->value.GetObject().MemberEnd()){
					StringW path = currentLevelPath;
					path = path + "/" + characteristicIconFilePath->value.GetString();
					sprite = UIUtils::FileToSprite(path);
				}
			}
		}


		bool SetColourFromIteratorString(const char16_t *name, Sombrero::FastColor& mapColour, CustomJSONData::ValueUTF16& customData){
			auto colorItr = customData.GetObject().FindMember(name);
			if (colorItr != customData.MemberEnd()) {
				if(colorItr->value.FindMember(u"r") == colorItr->value.MemberEnd()) return false;
				if(colorItr->value.FindMember(u"g") == colorItr->value.MemberEnd()) return false;
				if(colorItr->value.FindMember(u"b") == colorItr->value.MemberEnd()) return false;
				mapColour = { colorItr->value[u"r"].GetFloat() , colorItr->value[u"g"].GetFloat() , colorItr->value[u"b"].GetFloat(), 1.0f };
				return true;
			}
			return false;
		}

		GlobalNamespace::ColorScheme* GetCustomSongColour(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride) {
			CustomJSONData::ValueUTF16 customData;
			if(SongUtils::CustomData::GetCurrentCustomDataJson(SongUtils::GetCurrentInfoDat(), customData)){
				return SongUtils::CustomData::GetCustomSongColourFromCustomData(colorScheme, hasOverride, customData);
			}
			return nullptr;
		}


		GlobalNamespace::ColorScheme* GetCustomSongColourFromCustomData(GlobalNamespace::ColorScheme* colorScheme, bool hasOverride, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData) {

			Sombrero::FastColor colorLeft = colorScheme->saberAColor;
			Sombrero::FastColor colorRight = colorScheme->saberBColor;
			Sombrero::FastColor envColorLeft = colorScheme->environmentColor0;
			Sombrero::FastColor envColorRight = colorScheme->environmentColor1;
			Sombrero::FastColor envColorWhite = colorScheme->environmentColorW;
			Sombrero::FastColor envColorLeftBoost = colorScheme->environmentColor0Boost;
			Sombrero::FastColor envColorRightBoost = colorScheme->environmentColor1Boost;
			Sombrero::FastColor envColorWhiteBoost = colorScheme->environmentColorWBoost;
			Sombrero::FastColor obstacleColor = colorScheme->obstaclesColor;

			bool hasBoostColours = false;
			bool hasSaberColours = false;
			bool hasLightColours = false;
			bool hasObstacleColours = false;
			bool hasWhiteLightColours = false;
			if(SetColourFromIteratorString(u"_colorLeft", colorLeft, customData)) hasSaberColours = true;
			if(SetColourFromIteratorString(u"_colorRight", colorRight, customData)) hasSaberColours = true;
			if(SetColourFromIteratorString(u"_envColorLeft", envColorLeft, customData)) hasLightColours = true;
			if(SetColourFromIteratorString(u"_envColorRight", envColorRight, customData)) hasLightColours = true;
			if(SetColourFromIteratorString(u"_envColorWhite", envColorWhite, customData)) hasWhiteLightColours = true;
			if(SetColourFromIteratorString(u"_envColorLeftBoost", envColorLeftBoost, customData)) hasBoostColours = true;
			if(SetColourFromIteratorString(u"_envColorRightBoost", envColorRightBoost, customData)) hasBoostColours = true;
			if(SetColourFromIteratorString(u"_envColorWhiteBoost", envColorWhiteBoost, customData)) hasWhiteLightColours = true;
			if(SetColourFromIteratorString(u"_obstacleColor", obstacleColor, customData)) hasObstacleColours = true;

			if (hasSaberColours || hasLightColours || hasBoostColours || hasObstacleColours || hasWhiteLightColours) {
				if(hasSaberColours && !hasLightColours){
					envColorLeft = colorLeft;
					envColorRight = colorRight;
					hasLightColours = true;
				}
				if(hasLightColours && !hasBoostColours){
					envColorLeftBoost = envColorLeft;
					envColorRightBoost = envColorRight;
				}

				if(config.forceNoteColours && hasOverride){
					colorLeft = colorScheme->saberAColor;
					colorRight = colorScheme->saberBColor;
				}
				StringW colorSchemeId = "PinkCoreMapColorScheme";
				StringW colorSchemeNameLocalizationKey = "PinkCore Map Color Scheme";
				auto newColorScheme = GlobalNamespace::ColorScheme::New_ctor(colorSchemeId, colorSchemeNameLocalizationKey, true, colorSchemeNameLocalizationKey, false, colorLeft, colorRight, envColorLeft, envColorRight, envColorWhite, colorScheme->supportsEnvironmentColorBoost, envColorLeftBoost, envColorRightBoost, envColorWhiteBoost, obstacleColor);

				return newColorScheme;

			}
			else {
				return nullptr;
			}


		}

		void ExtractRequirements(const CustomJSONData::ValueUTF16& requirementsArray, std::vector<std::string>& output)
		{
			auto actualArray = requirementsArray.GetArray();
			for (auto& requirement : actualArray)
			{
				std::string requirementName = to_utf8(requirement.GetString());
				std::string requirementNameWithoutSpaces = removeSpaces(requirementName);

				auto it = std::find(output.begin(), output.end(), requirementName);

				if (it == output.end())
					output.push_back(requirementName);
			}

			std::sort(output.begin(), output.end());
		}

		bool MapHasColoursChecker(CustomJSONData::ValueUTF16& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{
			static const char16_t* colours[] = {u"_colorLeft", u"_colorRight",u"_envColorLeft", u"_envColorRight", u"_envColorLeftBoost", u"_envColorRightBoost", u"_obstacleColor"};
			for (auto name : colours) {
				auto itr = customData.GetObject().FindMember(name);
				if (itr != customData.MemberEnd()) {
					auto end = itr->value.MemberEnd();
					if(itr->value.FindMember(u"r") == end) continue;
					if(itr->value.FindMember(u"g") == end) continue;
					if(itr->value.FindMember(u"b") == end) continue;
					return true;
				}
			}
			return false;
		}

		int MapSaberCountChecker(CustomJSONData::ValueUTF16& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{
			auto itr = customData.GetObject().FindMember(u"_oneSaber");
			if (itr != customData.MemberEnd()) {
				if(itr->value.GetBool()){ //if one saber is true
					return 1; //only have 1 saber
				}else{
					return 2; //if its false, then we have 2 sabers
				}
			}
			return -1; //if the object doesnt exist, then we use -1 to specify that this option shouldnt be used
		}

		const char16_t* MapEnvironmentTypeChecker(CustomJSONData::ValueUTF16& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{

			auto itr = customData.GetObject().FindMember(u"_environmentType");
			if (itr != customData.MemberEnd()) {
				return itr->value.GetString(); //only have 1 saber
			}
			return u"Default"; //if the object doesnt exist, then we use Default to specify that this option shouldnt be used
		}

		bool MapShouldShowRotationSpawnLines(CustomJSONData::ValueUTF16& customData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic)
		{
			auto itr = customData.GetObject().FindMember(u"_showRotationNoteSpawnLines");
			if (itr != customData.MemberEnd()) {
				return itr->value.GetBool();
			}
			return true; //if the object doesnt exist, then we use true to specify that the base game option should still happen
		}
	}

	namespace SongInfo
	{
		PinkCore::API::LevelDetails currentMapLevelDetails;

		void ResetMapData(PinkCore::API::LevelDetails& mapData){
			mapData.environmentType = u"Default";
			mapData.hasCustomColours = false;
			mapData.isCustom = false;
			mapData.saberCount = -1; //-1 = No Data, dont do anything
			mapData.isWIP = false;
			mapData.showRotationSpawnLines = true;
			mapData.dataIsValid = false;
			RequirementUtils::EmptyRequirements(mapData);
			ContributorUtils::EmptyContributors(mapData);
		}

		void ResetMapData(){
			ResetMapData(currentMapLevelDetails);
		}

		void UpdateMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
			SetMapData(currentInfoDat, currentMapLevelDetails, difficultyBeatmap);
		}
		void UpdateMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, GlobalNamespace::IPreviewBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic){
			SetMapData(currentInfoDat, currentMapLevelDetails, level, difficulty, characteristic);
		}

		void SetMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, PinkCore::API::LevelDetails& mapData, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap){
			GlobalNamespace::BeatmapDifficulty difficulty = difficultyBeatmap->get_difficulty();
        	GlobalNamespace::BeatmapCharacteristicSO* characteristic = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic();
        	GlobalNamespace::IPreviewBeatmapLevel* level = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(difficultyBeatmap->get_level());
			SetMapData(currentInfoDat, mapData, level, difficulty, characteristic);
		}

		void SetMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, PinkCore::API::LevelDetails& mapData, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic){
			mapData.difficulty = difficulty;
			mapData.characteristic = characteristic;
			CustomJSONData::ValueUTF16 customData;
			if(SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(currentInfoDat, customData, difficulty, characteristic)){
				RequirementUtils::HandleRequirementDetails(mapData);
				ContributorUtils::FetchListOfContributors(mapData);
				mapData.environmentType = SongUtils::CustomData::MapEnvironmentTypeChecker(customData, difficulty, characteristic);
				mapData.hasCustomColours = SongUtils::CustomData::MapHasColoursChecker(customData, difficulty, characteristic);
				mapData.showRotationSpawnLines = SongUtils::CustomData::MapShouldShowRotationSpawnLines(customData, difficulty, characteristic);
				mapData.saberCount = SongUtils::CustomData::MapSaberCountChecker(customData, difficulty, characteristic);
				mapData.isCustom = true;
				mapData.dataIsValid = true;
			}else{
				SongInfo::ResetMapData(mapData);
			}
		}

		void SetMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& currentInfoDat, PinkCore::API::LevelDetails& mapData, GlobalNamespace::IPreviewBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic){
			bool mapIsCustom = false;
			if(level) mapIsCustom = isCustom(level);
			if(mapIsCustom){
				SetMapData(currentInfoDat, mapData, difficulty, characteristic);
				mapData.isCustom = mapIsCustom;
				mapData.isWIP = isWIP(level);
			}else{
				ResetMapData();
			}
		}



		bool isCustom(GlobalNamespace::IPreviewBeatmapLevel* level)
		{
			if (!level) return false;
			//had to resort to the code below again, multi for some reason shits itself here
			//return il2cpp_functions::class_is_assignable_from(classof(GlobalNamespace::CustomPreviewBeatmapLevel*), il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(level)));
			// the above check should suffice, but this code will remain as backup


			//the above did not suffice.
			//broke in multi
			//now the RSL makes this consistent, resorting back to this
			StringW levelidCS = level->get_levelID();
			return levelidCS->Contains("custom_level_");
		}

		bool isWIP(GlobalNamespace::IPreviewBeatmapLevel* level)
		{
			if (!level) return false;
			StringW levelidCS = level->get_levelID();
		    // if the level ID contains `WIP` then the song is a WIP song
			return levelidCS.ends_with(u" WIP");
		}


		PinkCore::API::LevelDetails& get_mapData()
		{
			return currentMapLevelDetails;
		}

		void set_mapIsCustom(bool val)
		{
			currentMapLevelDetails.isCustom = val;
		}

		void set_mapIsWIP(bool val)
		{
			currentMapLevelDetails.isWIP = val;
		}

	}
}
