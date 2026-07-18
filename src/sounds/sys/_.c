#include "sound_process_system.c"
#include "sound_generate_system.c"
#include "sound_debug_system.c"
#include "settings.c"
#ifdef zox_sdl_mixer
    #include "sound_play_system.c"
    #include "sound_play_ref_system.c"
#endif

void define_systems_sounds(ecs *world) {
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
        zoxp_mainthread,
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
        [out] sdl.SDLMixChunk,
        [none] Sound
    );
    zox_system(
        SoundPlayRefSystem,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundDataRef,
        [out] sdl.SDLMixChunk,
        [none] Sound
    );
#endif
    // Sound gen takes longer;
    zox_system_1(
        SoundsSettingsSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [none] apps.App
    );
    zox_system_1(
        SoundsSettingsDirtySystem,
        zoxp_mainthread,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
}
