#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"
#include "config.hpp"

#include "GlobalNamespace/MainSettingsModelSO.hpp"
#include "GlobalNamespace/SaberBurnMarkArea.hpp"
#include "GlobalNamespace/BoolSO.hpp"

MAKE_HOOK_MATCH(MainSettingsModelSO_Load, &GlobalNamespace::MainSettingsModelSO::Load, void, GlobalNamespace::MainSettingsModelSO* self, bool forced)
{
	self->burnMarkTrailsEnabled->set_value(true);
	MainSettingsModelSO_Load(self, forced); 
}

//stops the marks from spawning, this way it allows it to be configurable during runtime without a reload.
MAKE_HOOK_MATCH(SaberBurnMarkArea_LateUpdate, &GlobalNamespace::SaberBurnMarkArea::LateUpdate, void, GlobalNamespace::SaberBurnMarkArea* self)
{
	if (config.enableBurnMarks) {
		SaberBurnMarkArea_LateUpdate(self); 
	} else {
                self->get_camera()->Render();
        }
}

void InstallBurnMarkHooks(Logger& logger)
{
	SIMPLE_INSTALL_HOOK(MainSettingsModelSO_Load);
	SIMPLE_INSTALL_HOOK(SaberBurnMarkArea_LateUpdate);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallBurnMarkHooks)
