#include "process.c"
#include "generate.c"
#include "debug.c"
#include "settings.c"
#ifdef zox_sdl_mixer
    #ifdef zox_sdl3
        #include "play3.c"
    #else
        #include "play.c"
    #endif
#endif

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
#ifdef zox_sdl_mixer
    zox_system(
        SoundPlaySystem,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundData,
#ifndef zox_sdl3
        [out] sdl.SDLMixChunk,
#endif
        [none] Sound
    );
    zox_system(
        SoundPlayRefSystem,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundDataRef,
#ifndef zox_sdl3
        [out] sdl.SDLMixChunk,
#endif
        [none] Sound
    );
#endif
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
