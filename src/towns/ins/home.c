// a home in its town: its floor height (the ground under it is levelled to it), facing its door to the town's middle
entity spawn_home(ecs* world, entity prefab, entity parent, lint seed, int2 position, byte2 size, byte floor, byte facing) {
    zox_instance(prefab);
    {
        char name[64];
        sprintf(name, "home_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, parent);
    zox_set(e, Seed, { seed });
    zox_set(e, BlockPosition2, { position });
    zox_set(e, HomeSize, { size });
    zox_setv(e, HomeStyle, (byte) (seed % 3 != 0));   // two in three peaked, the rest flat
    zox_setv(e, HomeFloor, floor);
    zox_setv(e, HomeFacing, facing);
    return e;
}

// the way a home at position faces to look at a point (its door towards it, along the axis it's farther on)
static inline byte home_facing_to(int2 position, int2 to) {
    const int dx = to.x - position.x, dz = to.y - position.y;
    if (abs(dz) >= abs(dx)) {
        return dz >= 0 ? 0 : 2;
    }
    return dx >= 0 ? 1 : 3;
}
