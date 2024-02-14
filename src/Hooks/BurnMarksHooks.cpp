#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/BoolSO.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/LineRenderer.hpp"
#include "UnityEngine/Renderer.hpp"
#include "UnityEngine/Texture2D.hpp"
#include "UnityEngine/Material.hpp"

MAKE_AUTO_HOOK_MATCH(MainSettingsModelSO_Load, &GlobalNamespace::MainSettingsModelSO::Load, void, GlobalNamespace::MainSettingsModelSO* self, ::GlobalNamespace::ISaveData* saveData, bool forced) {
	MainSettingsModelSO_Load(self, saveData, forced);
	self->burnMarkTrailsEnabled->set_value(true);
}

//stops the marks from spawning, this way it allows it to be configurable during runtime without a reload.
MAKE_AUTO_HOOK_MATCH(SaberBurnMarkArea_LateUpdate, &GlobalNamespace::SaberBurnMarkArea::LateUpdate, void, GlobalNamespace::SaberBurnMarkArea* self) {
	if (config.enableBurnMarks) {
		SaberBurnMarkArea_LateUpdate(self);
	} else {
		self->_renderer->get_sharedMaterial()->set_mainTexture(UnityEngine::Texture2D::get_blackTexture());
    }
}
