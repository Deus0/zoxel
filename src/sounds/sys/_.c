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
        [none] Sound,
        [none] GenerateSound
    );
    add_system_process_counter(world, zox_id(sound_generate_system));
    zox_on_add(
        sound_debug_event,
        [in] SoundData,
        [none] Sound,
        [none] core.Dirty
    );
    zox_on_add(
        sound_play_event,
        [in] SoundData,
        [in] ?SoundPosition,
        [in] ?SoundDelay,
        [none] Sound,
        [none] core.Dirty
    );
    zox_on_add(
        sound_play_ref_event,
        [in] SoundDataRef,
        [in] ?SoundPosition,
        [none] Sound,
        [none] core.Dirty
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
