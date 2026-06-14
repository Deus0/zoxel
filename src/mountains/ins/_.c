entity spawn_mountain(ecs* world, entity prefab, entity parent, lint seed, int2 position, byte size, byte height) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "town_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, BlockPosition2, { position });
    zox_set(e, Radius, { size });
    zox_set(e, Height, { height });
    zox_set(e, Seed, { seed });
    return e;
}
