#include "process.c"
#include "generate.c"
#include "debug.c"
#include "settings.c"
#include "play.c"

void zox_systems_sounds(ecs *world) {
    zox_system(
        SoundProcessSystem,
        zoxp_update,
        [in] ProcessSound,
        [in] SoundData,
        [in] SoundVolume,
        [in] SoundFrequency,
        [out] TriggerSound,
        [None] Sound
    );
    zox_system(
        SoundGenerateSystem,
        zoxp_update,
        [in] InstrumentType,
        [in] SoundLength,
        [in] SoundFrequency,
        [in] SoundVolume,
        [in] GenerateSound,
        [in] SoundData,
        [out] TriggerSound,
        [none] Sound
    );
    add_system_process_counter(world, zox_id(SoundGenerateSystem));
    zox_system_1(
        SoundDebugSystem,
        zoxp_spawn,
        [in] SoundData,
        [in] TriggerSound,
        [none] Sound
    );
    zox_system(
        SoundPlaySystem,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundData,
        [in] ?SoundPosition,
        [none] Sound
    );
    zox_system(
        SoundPlayRefSystem,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundDataRef,
        [in] ?SoundPosition,
        [none] Sound
    );
    zox_system_1(
        SoundListenerSystem,
        zoxp_update,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] SoundListener
    );
    // Sound gen takes longer;
    zox_system_1(
        SoundsSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings
    );
    zox_system(
        SoundsSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
