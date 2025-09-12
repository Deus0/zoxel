

void spawn_prefabs_characters(ecs* world) {
    prefabs_add_characters_to_player(world, prefab_player);
    if (prefab_camera_game) {
        zox_prefab_set(prefab_camera_game, CharacterLink, { 0 });
    }
}