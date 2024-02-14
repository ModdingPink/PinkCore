#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "UnityEngine/Vector2.hpp"

#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/StaticBeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/NoteCutDirectionExtensions.hpp"
#include "GlobalNamespace/Vector2Extensions.hpp"

#include "sombrero/shared/linq_functional.hpp"

using namespace GlobalNamespace;

static inline UnityEngine::Vector2 operator -(UnityEngine::Vector2 a, UnityEngine::Vector2 b) {
    return { a.x - b.x, a.y - b.y };
}

template<typename T>
auto OfType(auto range) {
    using namespace Sombrero::Linq::Functional;

    return range | Where([](auto i) {
        return i != nullptr && il2cpp_utils::AssignableFrom<T>(reinterpret_cast<Il2CppObject*>(i)->klass);
    }) | Select([](auto i) {
        return reinterpret_cast<T>(i);
    });
}

void BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSliceTranspile(BeatmapObjectsInTimeRowProcessor* self,
               GlobalNamespace::BeatmapObjectsInTimeRowProcessor::TimeSliceContainer_1<::GlobalNamespace::BeatmapDataItem*>* allObjectsTimeSlice, float nextTimeSliceTime) {
    auto notesInColumnsReusableProcessingListOfLists = self->_notesInColumnsReusableProcessingListOfLists;
    for (auto const &l: notesInColumnsReusableProcessingListOfLists) {
        l->Clear();
    }

    auto items = ListWrapper<BeatmapDataItem*>(allObjectsTimeSlice->get_items());

    // Removing usage of OfType / Linq for now since it caused crashes with v3 maps
    //auto enumerable = OfType<NoteData *>(items);
    //auto enumerable2 = OfType<SliderData *>(items);
    //auto enumerable3 = OfType<BeatmapObjectsInTimeRowProcessor::SliderTailData *>(items);

    // used to be enumerable
    for (auto data : items) {
        auto noteData = il2cpp_utils::try_cast<NoteData>(data).value_or(nullptr);
        if (!noteData) continue;

        // TRANSPILE HERE
        // CLAMP
        ListW<GlobalNamespace::NoteData*> list {self->_notesInColumnsReusableProcessingListOfLists[std::clamp(noteData->lineIndex, 0, 3)]};
        // TRANSPILE HERE

        bool flag = false;

        for (int j = 0; j < list.size(); j++) {
            if (list[j]->noteLineLayer > noteData->noteLineLayer) {
                list->Insert(j, noteData);
                flag = true;
                break;
            }
        }
        if (!flag) {
            list->Add(noteData);
        }
    }
    for (auto const& notesInColumnsReusableProcessingListOfList : self->_notesInColumnsReusableProcessingListOfLists) {
        ListW<GlobalNamespace::NoteData*> list2{notesInColumnsReusableProcessingListOfList};
        for (int l = 0; l < list2.size(); l++) {
            list2[l]->SetBeforeJumpNoteLineLayer((NoteLineLayer) l);
        }
    }
    // used to be enumerable2
    for (auto data : items) {
        auto sliderData = il2cpp_utils::try_cast<SliderData>(data).value_or(nullptr);
        if (!sliderData) continue;
        // used to be enumerable
        for (auto data : items) {
            auto noteData2 = il2cpp_utils::try_cast<NoteData>(data).value_or(nullptr);
            if (!noteData2) continue;

            if (BeatmapObjectsInTimeRowProcessor::SliderHeadPositionOverlapsWithNote(sliderData, noteData2)) {
                sliderData->SetHasHeadNote(true);
                sliderData->SetHeadBeforeJumpLineLayer(noteData2->beforeJumpNoteLineLayer);
                if (sliderData->sliderType == SliderData::Type::Burst) {
                    noteData2->ChangeToBurstSliderHead();
                    if (noteData2->cutDirection == sliderData->tailCutDirection) {
                        auto line = StaticBeatmapObjectSpawnMovementData::Get2DNoteOffset(noteData2->lineIndex,
                                                                                          self->_numberOfLines,
                                                                                          noteData2->noteLineLayer) -
                                    StaticBeatmapObjectSpawnMovementData::Get2DNoteOffset(sliderData->tailLineIndex,
                                                                                          self->_numberOfLines,
                                                                                          sliderData->tailLineLayer);
                        float num = Vector2Extensions::SignedAngleToLine(
                                NoteCutDirectionExtensions::Direction(noteData2->cutDirection), line);
                        if (std::abs(num) <= 40.0f) {
                            noteData2->SetCutDirectionAngleOffset(num);
                            sliderData->SetCutDirectionAngleOffset(num, num);
                        }
                    }
                } else {
                    noteData2->ChangeToSliderHead();
                }
            }
        }
    }
    // used to be enumerable3
    for (auto data : items) {
        auto sliderTailData = il2cpp_utils::try_cast<BeatmapObjectsInTimeRowProcessor::SliderTailData>(data).value_or(nullptr);
        if (!sliderTailData) continue;
        auto slider = sliderTailData->slider;
        // used to be enumerable
        for (auto data : items) {
            auto noteData3 = il2cpp_utils::try_cast<NoteData>(data).value_or(nullptr);
            if (!noteData3) continue;

            if (BeatmapObjectsInTimeRowProcessor::SliderTailPositionOverlapsWithNote(slider, noteData3)) {
                slider->SetHasTailNote(true);
                slider->SetTailBeforeJumpLineLayer(noteData3->beforeJumpNoteLineLayer);
                noteData3->ChangeToSliderTail();
            }
        }

    }
}

MAKE_AUTO_HOOK_ORIG_MATCH(BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSlice,
                &BeatmapObjectsInTimeRowProcessor::HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSlice,
                void,
                BeatmapObjectsInTimeRowProcessor* self,
                GlobalNamespace::BeatmapObjectsInTimeRowProcessor::TimeSliceContainer_1<::GlobalNamespace::BeatmapDataItem*>* allObjectsTimeSlice, float nextTimeSliceTime) {
    return BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSliceTranspile(self, allObjectsTimeSlice, nextTimeSliceTime);
}
