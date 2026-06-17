entity spawn_streamer(ecs *world, entity prefab, entity terrain, float3 position){
    zox_instance(prefab);
    zox_name("streamer");
    zox_set(e, StreamLink, { terrain });
    zox_set(e, Position3D, { position });
    // zox_set(e, StreamPosition, { point });
    // zox_set(e, StreamPosition2, { { point.x, point.z } });
    return e;
}
