#include "beatsaber-hook/shared/config/rapidjson-utils.hpp"

#include "Utils/SongUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "GlobalNamespace/CustomBeatmapLevel.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/FilteredBeatmapLevel.hpp"
#include "logging.hpp"
#include "config.hpp"

#include "songloader/shared/CustomTypes/CustomLevelInfoSaveData.hpp"
#include "sombrero/shared/FastColor.hpp"
#include "Utils/UIUtils.hpp"
#include <fstream>
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

	bool currentInfoDatValid = false;

	std::u16string lastPhysicallySelectedCharacteristic = u"";
	std::u16string lastPhysicallySelectedDifficulty = u"";
	std::u16string currentLevelPath = u"";
	
	bool currrentlySelectedIsCustom = false;
	bool currrentlySelectedIsWIP = false;

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

	std::u16string GetDiffFromNumber(GlobalNamespace::BeatmapDifficulty selectedDifficulty)
	{
		switch (selectedDifficulty.value)
		{
			case 0:
				return u"Easy";
				break;
			case 1:
				return u"Normal";
				break;
			case 2:
				return u"Hard";
				break;
			case 3:
				return u"Expert";
				break;
			case 4:
				return u"ExpertPlus";
				break;
			default:
				return u"Unknown";
				break;
		}
	}

	GlobalNamespace::BeatmapDifficulty GetNumberFromDiff(std::u16string difficulty)
	{
		// another way of doing it with a map
		/*
		static std::map<std::string, GlobalNamespace::BeatmapDifficulty> stringToDiff = {
			{ "Easy", GlobalNamespace::BeatmapDifficulty::Easy },
			{ "Normal", GlobalNamespace::BeatmapDifficulty::Normal },
			{ "Hard", GlobalNamespace::BeatmapDifficulty::Hard },
			{ "Expert", GlobalNamespace::BeatmapDifficulty::Expert },
			{ "ExpertPlus", GlobalNamespace::BeatmapDifficulty::ExpertPlus }
		};

		auto itr = stringToDiff.find(difficulty);
		if (itr != stringToDiff.end()) return itr->second;
		else return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		return stringToDiff[difficulty];		
		*/
		if (difficulty == u"Easy") return GlobalNamespace::BeatmapDifficulty::Easy;
		else if (difficulty == u"Normal") return GlobalNamespace::BeatmapDifficulty::Normal;
		else if (difficulty == u"Hard") return GlobalNamespace::BeatmapDifficulty::Hard;
		else if (difficulty == u"Expert") return GlobalNamespace::BeatmapDifficulty::Expert;
		else return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
	}

	namespace CustomData
	{
		
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomBeatmapLevel* customLevel;
			if (auto filter = il2cpp_utils::try_cast<GlobalNamespace::FilteredBeatmapLevel>(level)) {
				customLevel = il2cpp_utils::cast<GlobalNamespace::CustomBeatmapLevel>(filter.value()->beatmapLevel);
			} else {
				customLevel = il2cpp_utils::cast<GlobalNamespace::CustomBeatmapLevel>(level);
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
				return !doc->GetParseError();
			}
		}
		
		/*
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = reinterpret_cast<GlobalNamespace::CustomPreviewBeatmapLevel*>(level);

			std::string songPath(customLevel->get_customLevelPath());
			currentLevelPath = songPath;

			auto infoDataOpt = il2cpp_utils::try_cast<CustomJSONData::CustomLevelInfoSaveData>(customLevel->get_standardLevelInfoSaveData());
			// if an info.dat already exists on the given level, don't read the file again
			if (infoDataOpt)
			{
				INFO("Found custom json data on level");
				auto infoData = infoDataOpt.value();
				doc.CopyFrom(*infoData->doc, doc.GetAllocator());
				return true;
			}
			else
			{
				songPath += "/info.dat";
				// get the path
				INFO("Getting info.dat for %s", songPath.c_str());
				// if the file doesnt exist, fail
				if (!fileexists(songPath)) return false;

				INFO("Reading file");
				// read file
				std::string info = readfile(songPath);

				// parse into doc
				doc.Parse(info.c_str());
				// return true if it read the file right, return false if there was a parse error
				return !doc.GetParseError();
			}
		}*/

		bool GetCurrentCustomData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& out)
		{
			return GetCurrentCustomData(in, out, GetNumberFromDiff(SongUtils::SongInfo::get_lastPhysicallySelectedDifficulty()));
		}

		bool GetCurrentCustomData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& out, GlobalNamespace::BeatmapDifficulty difficulty)
		{
			bool hasCustomData = false;
			auto& lastPhysicallySelectedCharacteristic = SongUtils::SongInfo::get_lastPhysicallySelectedCharacteristic();
			std::u16string difficultyToFind = SongUtils::GetDiffFromNumber(difficulty);
			INFO("Looking for characteristic: %s", to_utf8(difficultyToFind).c_str());
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
					if (beatmapCharacteristicName == lastPhysicallySelectedCharacteristic)
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
			return hasCustomData;
		}



		void GetCustomCharacteristicItems(StringW characteristic, UnityEngine::Sprite*& sprite, StringW& hoverText){
			auto& d = SongUtils::GetCurrentInfoDat();
			bool hasCustomData = false;

			auto difficultyBeatmapSetsitr = d.FindMember(u"_difficultyBeatmapSets");
			// if we find the sets iterator
			if (difficultyBeatmapSetsitr != d.MemberEnd())
			{
				auto setArr = difficultyBeatmapSetsitr->value.GetArray();
				for (auto& beatmapCharacteristicItr : setArr)
				{
					std::u16string beatmapCharacteristicName = beatmapCharacteristicItr.FindMember(u"_beatmapCharacteristicName")->value.GetString();
					INFO("Found CharacteristicName: %s", (char*)beatmapCharacteristicName.c_str());
					// if the last selected beatmap characteristic is this specific one
					if (beatmapCharacteristicName == characteristic)
					{
						auto customDataItr = beatmapCharacteristicItr.GetObject().FindMember(u"_customData");
						if(customDataItr != beatmapCharacteristicItr.MemberEnd()){		
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
				}
			}
		}

	void SetCharacteristicAndDifficulty(GlobalNamespace::BeatmapDifficulty beatmapDifficulty, GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic){
		auto serializedNameCS = beatmapCharacteristic ? beatmapCharacteristic->get_serializedName() : nullptr;
		std::u16string serializedName(serializedNameCS ? serializedNameCS : u"");
		SongUtils::SongInfo::set_lastPhysicallySelectedCharacteristic(serializedName);
		SongUtils::SongInfo::set_lastPhysicallySelectedDifficulty(SongUtils::GetDiffFromNumber(beatmapDifficulty));
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

	GlobalNamespace::ColorScheme* GetCustomSongColour(GlobalNamespace::ColorScheme* defaultColorScheme, bool hasOverride) {

		auto& d = SongUtils::GetCurrentInfoDat();
		rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
		if (SongUtils::CustomData::GetCurrentCustomData(d, customData)) {
			
			Sombrero::FastColor colorLeft = defaultColorScheme->saberAColor;
			Sombrero::FastColor colorRight = defaultColorScheme->saberBColor;
			Sombrero::FastColor envColorLeft = defaultColorScheme->environmentColor0;
			Sombrero::FastColor envColorRight = defaultColorScheme->environmentColor1;
			Sombrero::FastColor envColorLeftBoost = defaultColorScheme->environmentColor0Boost;
			Sombrero::FastColor envColorRightBoost = defaultColorScheme->environmentColor1Boost;
			Sombrero::FastColor obstacleColor = defaultColorScheme->obstaclesColor;
			
			bool hasBoostColours = false;
			bool hasSaberColours = false;
			bool hasLightColours = false;
			bool hasObstacleColours = false;
			if(SetColourFromIteratorString(u"_colorLeft", colorLeft, customData)) hasSaberColours = true;
			if(SetColourFromIteratorString(u"_colorRight", colorRight, customData)) hasSaberColours = true;
			if(SetColourFromIteratorString(u"_envColorLeft", envColorLeft, customData)) hasLightColours = true;
			if(SetColourFromIteratorString(u"_envColorRight", envColorRight, customData)) hasLightColours = true;
			if(SetColourFromIteratorString(u"_envColorLeftBoost", envColorLeftBoost, customData)) hasBoostColours = true; 
			if(SetColourFromIteratorString(u"_envColorRightBoost", envColorRightBoost, customData)) hasBoostColours = true; 
			if(SetColourFromIteratorString(u"_obstacleColor", obstacleColor, customData)) hasObstacleColours = true;

			if (hasSaberColours || hasLightColours || hasBoostColours || hasObstacleColours) {
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
					colorLeft = defaultColorScheme->saberAColor;
					colorRight = defaultColorScheme->saberBColor;
				}
				StringW colorSchemeId = "PinkCoreMapColorScheme";
				StringW colorSchemeNameLocalizationKey = "PinkCore Map Color Scheme";
				auto newColorScheme = *il2cpp_utils::New<GlobalNamespace::ColorScheme*>(colorSchemeId, colorSchemeNameLocalizationKey, true, colorSchemeNameLocalizationKey, false, colorLeft, colorRight, envColorLeft, envColorRight, defaultColorScheme->supportsEnvironmentColorBoost, envColorLeftBoost, envColorRightBoost, obstacleColor);


				return newColorScheme;

			}
			else {
				return nullptr;
			}
		}
		else {
			return nullptr;
		}
	}

	void ExtractRequirements(const rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& requirementsArray, std::vector<std::string>& output)
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
}

	namespace SongInfo
	{
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
			if (levelidCS->Contains("custom_level_")) return true;

			return false;
			
		}

		/*-------------------------------------------------*/

		bool get_currentlySelectedIsCustom()
		{
			return currrentlySelectedIsCustom;
		}

		void set_currentlySelectedIsCustom(bool val)
		{
			currrentlySelectedIsCustom = val;
		}
		/*-------------------------------------------------*/


		bool get_currentlySelectedHasColours()
		{
			auto& d = SongUtils::GetCurrentInfoDat();
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			if (SongUtils::CustomData::GetCurrentCustomData(d, customData)) {
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
			}
			return false;
		}

		// void set_currentlySelectedHasColours(bool val)
		// {
		// 	currentlySelectedHasColours = val;
		// }
		/*-------------------------------------------------*/

		bool get_currentlySelectedIsWIP()
		{
			return currrentlySelectedIsWIP;
		}

		void set_currentlySelectedIsWIP(bool val)
		{
			currrentlySelectedIsWIP = val;
		}
		/*-------------------------------------------------*/

		bool get_currentInfoDatValid()
		{
			return currentInfoDatValid;
		}

		void set_currentInfoDatValid(bool value)
		{
			currentInfoDatValid = value;
			onLoadedInfoEvent.invoke(getOptional(value));
		}
		
		/*-------------------------------------------------*/

		const std::u16string& get_lastPhysicallySelectedCharacteristic()
		{
			return lastPhysicallySelectedCharacteristic;
		}

		void set_lastPhysicallySelectedCharacteristic(std::u16string_view value)
		{
			lastPhysicallySelectedCharacteristic = value;
		}
		/*-------------------------------------------------*/

		const std::u16string& get_lastPhysicallySelectedDifficulty()
		{
			return lastPhysicallySelectedDifficulty;
		}

		void set_lastPhysicallySelectedDifficulty(std::u16string_view value)
		{
			lastPhysicallySelectedDifficulty = value;
		}
	}
}