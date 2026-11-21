#include "process.c"
#include "generate.c"
#include "debug.c"
#include "settings.c"
#include "play.c"

void zox_systems_sounds(ecs *world) {
    zox_system(
        sound_process_system,
        zoxp_update,
        [in] SoundData,
        [in] SoundVolume,
        [in] SoundFrequency,
        [out] TriggerSound,
        [None] Sound,
        [none] ProcessSound
    );
    zox_system(
        sound_generate_system,
        zoxp_update,
        [in] InstrumentType,
        [in] SoundLength,
        [in] SoundFrequency,
        [in] SoundVolume,
        [in] SoundData,
        [out] TriggerSound,
        [none] Sound,
        [none] GenerateSound
    );
    add_system_process_counter(world, zox_id(sound_generate_system));
    zox_system_1(
        sound_debug_system,
        zoxp_spawn,
        [in] SoundData,
        [in] TriggerSound,
        [none] Sound
    );
    zox_system(
        sound_play_system,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundData,
        [in] ?SoundPosition,
        [in] ?SoundDelay,
        [none] Sound
    );
    zox_system(
        sound_play_ref_system,
        zoxp_update,
        [in] TriggerSound,
        [in] SoundDataRef,
        [in] ?SoundPosition,
        [none] Sound
    );
    // main thread (the mixer's listener), in zoxp_render with the other main thread systems: no merges of its
    //  own, and the camera's final position this frame
    zox_system_1(
        sound_listener_system,
        zoxp_render,
        [in] transforms3.Position3D,
        [in] transforms3.Rotation3D,
        [none] SoundListener
    );
    // Sound gen takes longer;
    // its settings: spawned on the app, applied when one changes (needs prefab_app)
    add_hook_spawn_prefabs(sounds_settings_events);
}
