// Implementation by https://github.com/StackDoubleFlow
#include "CustomTypes/CustomLevelInfoSaveData.hpp"

using namespace GlobalNamespace;
using namespace CustomJSONData;

DEFINE_TYPE(CustomJSONData, CustomLevelInfoSaveData);

void CustomLevelInfoSaveData::ctor(Il2CppString *songName, Il2CppString *songSubName, 
				 Il2CppString *songAuthorName, Il2CppString *levelAuthorName, float beatsPerMinute,
				 float songTimeOffset, float shuffle, float shufflePeriod, float previewStartTime, 
				 float previewDuration, Il2CppString *songFilename, Il2CppString *coverImageFilename, 
				 Il2CppString *environmentName, Il2CppString *allDirectionsEnvironmentName, 
				 Array<GlobalNamespace::StandardLevelInfoSaveData::DifficultyBeatmapSet*> *difficultyBeatmapSets) {
	INVOKE_CTOR();
	static auto* ctor = il2cpp_utils::FindMethodUnsafe("", "StandardLevelInfoSaveData", ".ctor", 15);
	CRASH_UNLESS(il2cpp_utils::RunMethod(this, ctor, songName, songSubName, songAuthorName, levelAuthorName, 
										 beatsPerMinute, songTimeOffset, shuffle, shufflePeriod, previewStartTime, 
										 previewDuration, songFilename, coverImageFilename, environmentName, allDirectionsEnvironmentName,
										 difficultyBeatmapSets));
}

DEFINE_TYPE(CustomJSONData, CustomDifficultyBeatmap);

void CustomDifficultyBeatmap::ctor(Il2CppString *difficultyName, int difficultyRank, Il2CppString *beatmapFilename, 
								   float noteJumpMovementSpeed, float noteJumpStartBeatOffset) {
	INVOKE_CTOR();
	this->difficulty = difficultyName;
	this->difficultyRank = difficultyRank;
	this->beatmapFilename = beatmapFilename;
	this->noteJumpMovementSpeed = noteJumpMovementSpeed;
	this->noteJumpStartBeatOffset = noteJumpStartBeatOffset;
}