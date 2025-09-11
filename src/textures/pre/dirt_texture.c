
entity spawn_texture_dirt(ecs *world, const entity prefab, const int seed, const color fill_color) {
    zox_instance(prefab)
    zox_name("texture")
    zox_set(e, Seed, { seed })
    zox_set(e, Color, { fill_color })
    return e;
}
