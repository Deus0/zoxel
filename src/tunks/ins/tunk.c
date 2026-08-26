// TODO: Add quadtree data and chunk2 tag
entity spawn_tunk(
    ecs* world,
    entity prefab,
    entity terrain,
    entity region,
    int2 position,
    byte distance,
    byte lod)
{
    zox_instance(prefab);
    {
        char name[32];
        sprintf(name, "tunk_%ix%i", position.x, position.y);
        zox_name(name);
    }
    zox_set_parent(world, e, terrain);
    zox_set(e, RegionLink, { region });
    zox_set(e, TunkPosition, { position });
    zox_set(e, RenderDistance, { distance });
    zox_set(e, TunkLod, { lod });
    return e;
}
