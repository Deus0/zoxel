// TODO: Add quadtree data and chunk2 tag
entity spawn_tunk(ecs* world, entity prefab, entity parent, int2 position, byte rdistance) {
    zox_instance(prefab);
    {
        char name[32];
        sprintf(name, "tunk_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, VoxLink, { parent });
    zox_set(e, TunkPosition, { position });
    zox_set(e, RenderDistance, { rdistance });
    // zox_log("+ Tunk2 [%ix%i]", position.x, position.y);
    return e;
}
