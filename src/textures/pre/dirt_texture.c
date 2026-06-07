entity spawn_texture_dirt(ecs *world, entity p, int seed, color fill_color) {
    zox_instance(p);
    zox_name("texture");
    zox_set(e, Seed, { seed });
    zox_set(e, FillColor, { fill_color });
    return e;
}
