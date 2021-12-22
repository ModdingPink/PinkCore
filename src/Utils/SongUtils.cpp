#include "Utils/SongUtils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "logging.hpp"

#include "CustomTypes/CustomLevelInfoSaveData.hpp"

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

static std::string removeSpaces(std::string input)
{
	std::string output = "";
	for (auto c : input)
	{
		if (c == ' ') continue;
		output += c;
	}
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
	bool currrentlySelectedIsNoodle = false;

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
		if (difficulty == u"Easy") {
			return GlobalNamespace::BeatmapDifficulty::Easy;
		}
		else if (difficulty == u"Normal") {
			return GlobalNamespace::BeatmapDifficulty::Normal;
		}
		else if (difficulty == u"Hard") {
			return GlobalNamespace::BeatmapDifficulty::Hard;
		}
		else if (difficulty == u"Expert") {
			return GlobalNamespace::BeatmapDifficulty::Expert;
		}
		else if (difficulty == u"ExpertPlus") {
			return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		}
		else {
			return GlobalNamespace::BeatmapDifficulty::ExpertPlus;
		}
	}

	namespace CustomData
	{
		
		bool GetInfoJson(GlobalNamespace::IPreviewBeatmapLevel* level, std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>& doc)
		{
			// if level nullptr or not custom it fails
			if (!level || !SongInfo::isCustom(level)) return false;

			// cast to custom level
			GlobalNamespace::CustomPreviewBeatmapLevel* customLevel = reinterpret_cast<GlobalNamespace::CustomPreviewBeatmapLevel*>(level);

			Il2CppString* levelPathCS = customLevel->get_customLevelPath();
			std::u16string songPath(csstrtostr(levelPathCS));
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

			Il2CppString* levelPathCS = customLevel->get_customLevelPath();
			std::string songPath = to_utf8(csstrtostr(levelPathCS));
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

		GlobalNamespace::ColorScheme* getCustomSongColour(GlobalNamespace::ColorScheme* defaultColorScheme, bool hasOverride) {

			auto& d = SongUtils::GetCurrentInfoDat();
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			if (SongUtils::CustomData::GetCurrentCustomData(d, customData)) {

				bool mapHasCustomColours = false;

				Il2CppString* colorSchemeId = il2cpp_utils::newcsstr("PinkCoreMapColorScheme");
				Il2CppString* colorSchemeNameLocalizationKey = il2cpp_utils::newcsstr("PinkCore Map Color Scheme");
				bool useNonLocalizedName = true;
				Il2CppString* nonLocalizedName = colorSchemeNameLocalizationKey;
				bool isEditable = false;
				UnityEngine::Color colorLeft = defaultColorScheme->saberAColor;
				UnityEngine::Color colorRight = defaultColorScheme->saberBColor;
				UnityEngine::Color envColorLeft = defaultColorScheme->environmentColor0;
				UnityEngine::Color envColorRight = defaultColorScheme->environmentColor1;
				bool supportsEnvironmentColorBoost = defaultColorScheme->supportsEnvironmentColorBoost;
				UnityEngine::Color envColorLeftBoost = defaultColorScheme->environmentColor0Boost;
				UnityEngine::Color envColorRightBoost = defaultColorScheme->environmentColor1Boost;
				UnityEngine::Color obstacleColor = defaultColorScheme->obstaclesColor;

				bool customBoostColours = false;

				auto colorLeftItr = customData.FindMember(u"_colorLeft");
				if (colorLeftItr != customData.MemberEnd()) {
					colorLeft = { colorLeftItr->value[u"r"].GetFloat() , colorLeftItr->value[u"g"].GetFloat() , colorLeftItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto colorRightItr = customData.FindMember(u"_colorRight");
				if (colorRightItr != customData.MemberEnd()) {
					colorRight = { colorRightItr->value[u"r"].GetFloat() , colorRightItr->value[u"g"].GetFloat() , colorRightItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorLeftItr = customData.FindMember(u"_envColorLeft");
				if (envColorLeftItr != customData.MemberEnd()) {
					envColorLeft = { envColorLeftItr->value[u"r"].GetFloat() , envColorLeftItr->value[u"g"].GetFloat() , envColorLeftItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorRightItr = customData.FindMember(u"_envColorRight");
				if (envColorRightItr != customData.MemberEnd()) {
					envColorRight = { envColorRightItr->value[u"r"].GetFloat() , envColorRightItr->value[u"g"].GetFloat() , envColorRightItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				auto envColorLeftBoostItr = customData.FindMember(u"_envColorLeftBoost");
				if (envColorLeftBoostItr != customData.MemberEnd()) {
					envColorLeftBoost = { envColorLeftBoostItr->value[u"r"].GetFloat() , envColorLeftBoostItr->value[u"g"].GetFloat() , envColorLeftBoostItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
					customBoostColours = true;
				}

				auto envColorRightBoostItr = customData.FindMember(u"_envColorRightBoost");
				if (envColorRightBoostItr != customData.MemberEnd()) {
					envColorRightBoost = { envColorRightBoostItr->value[u"r"].GetFloat() , envColorRightBoostItr->value[u"g"].GetFloat() , envColorRightBoostItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
					customBoostColours = true;
				}

				auto obstacleColorItr = customData.FindMember(u"_obstacleColor");
				if (obstacleColorItr != customData.MemberEnd()) {
					obstacleColor = { obstacleColorItr->value[u"r"].GetFloat() , obstacleColorItr->value[u"g"].GetFloat() , obstacleColorItr->value[u"b"].GetFloat(), 1.0f };
					mapHasCustomColours = true;
				}

				if (mapHasCustomColours) {
					//if an override is not set then this is null
					if (hasOverride) {
						colorLeft = defaultColorScheme->saberAColor;
						colorRight = defaultColorScheme->saberBColor;
					}

					if (!customBoostColours && !supportsEnvironmentColorBoost) {
						envColorLeftBoost = envColorLeft;
						envColorRightBoost = envColorRight;
					}

					auto newColorScheme = *il2cpp_utils::New<GlobalNamespace::ColorScheme*>(colorSchemeId, colorSchemeNameLocalizationKey, useNonLocalizedName, nonLocalizedName, isEditable, colorLeft, colorRight, envColorLeft, envColorRight, supportsEnvironmentColorBoost, envColorLeftBoost, envColorRightBoost, obstacleColor);

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
			return il2cpp_functions::class_is_assignable_from(classof(GlobalNamespace::CustomPreviewBeatmapLevel*), il2cpp_functions::object_get_class(reinterpret_cast<Il2CppObject*>(level)));
			// the above check should suffice, but this code will remain as backup
			/*
			Il2CppString* levelidCS = level->get_levelID();
			static Il2CppString* custom_level_ = il2cpp_utils::newcsstr<il2cpp_utils::CreationType::Manual>("custom_level_");
			// does level ID contain custom_level_ ?
			if (levelidCS->Contains(custom_level_)) return true;
			// fallback to _ on pos 45
			else if (static_cast<char>(levelidCS->get_Chars(45)) == '_') return true;
			return false;
			*/
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

		bool get_currentlySelectedIsWIP()
		{
			return currrentlySelectedIsWIP;
		}

		void set_currentlySelectedIsWIP(bool val)
		{
			currrentlySelectedIsWIP = val;
		}
		/*-------------------------------------------------*/

		bool get_currentlySelectedIsNoodle()
		{
			return currrentlySelectedIsNoodle;
		}

		void set_currentlySelectedIsNoodle(bool val)
		{
			currrentlySelectedIsNoodle = val;
		}

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