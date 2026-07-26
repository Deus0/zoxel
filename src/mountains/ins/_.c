entity spawn_mountain(ecs* world, entity prefab, entity parent, lint seed, byte type, int2 position, byte radius, byte height) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "mountain_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_setv(e, MountainType, type);
    zox_setv(e, BlockPosition2, position);
    zox_setv(e, Radius, radius);
    zox_setv(e, Height, height);
    zox_setv(e, Seed, seed);
    return e;
}
