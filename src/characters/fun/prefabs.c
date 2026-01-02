void prefabs_add_characters_to_player(ecs *world, const entity e) {
    zox_prefab_set(e, CharacterLink, { 0 })
    zox_prefab_set(e, CameraLink, { 0 })
}
