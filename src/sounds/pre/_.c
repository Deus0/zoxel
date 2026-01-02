entity prefab_sound;
entity prefab_sound_generated;
#include "sound.c"
#include "sound_filepath.c"
#include "sound_file_instance.c"
#include "sound_generated.c"
entity prefab_sound_filepath;

void spawn_prefabs_sounds(ecs *world) {
    prefab_sound_filepath = spawn_prefab_sound_filepath(world);
    prefab_sound = spawn_prefab_sound(world);
    prefab_sound_generated = spawn_prefab_generated_sound(world, prefab_sound);
}