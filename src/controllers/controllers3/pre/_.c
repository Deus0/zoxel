#include "character3_player.c"

entity prefab_character3_player;

void spawn_prefabs_controllers3D(ecs *world) {
    prefab_character3_player = spawn_prefab_character3_player(
        world,
        prefab_character3_skeleton
    );
    prefabs_characters3[prefab_characters_count++] = prefab_character3_player;
}
