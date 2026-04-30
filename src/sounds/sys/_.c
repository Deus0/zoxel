#include "sound_process_system.c"
#include "sound_generate_system.c"
#include "sound_debug_system.c"

#ifdef zox_sdl_mixer
    #include "sound_play_system.c"
    #include "sound_play_ref_system.c"
#endif

void define_systems_sounds(ecs *world) {
    zox_system(
        SoundProcessSystem,
        EcsOnUpdate,
        [in] ProcessSound,
        [in] SoundData,
        [in] SoundVolume,
        [in] SoundFrequency,
        [out] TriggerSound,
        [None] Sound
    );
    zox_system(
        SoundGenerateSystem,
        EcsOnUpdate,
        [in] InstrumentType,
        [in] SoundLength,
        [in] SoundFrequency,
        [in] SoundVolume,
        [in] GenerateSound,
        [in] SoundData,
        [out] TriggerSound,
        [none] Sound
    );
    zox_system_1(
        SoundDebugSystem,
        zoxp_mainthread,
        [in] SoundData,
        [in] TriggerSound,
        [none] Sound
    );

#ifdef zox_sdl_mixer
    if (!headless) {
        zox_system(
            SoundPlaySystem,
            EcsOnUpdate,
            [in] TriggerSound,
            [in] SoundData,
            [out] sdl.SDLMixChunk,
            [none] Sound
        );
        zox_system(
            SoundPlayRefSystem,
            EcsOnUpdate,
            [in] TriggerSound,
            [in] SoundDataRef,
            [out] sdl.SDLMixChunk,
            [none] Sound
        );
    }
#endif

    // Sound gen takes longer
    zox_set(zox_id(SoundGenerateSystem), SystemDeltaMax, { 5 });
}
