// based off cube prefab
entity spawn_prefab_pickup(ecs *world, entity prefab) {
    float pickup_radius = 0.15f;
    float4 rotation_speed = quaternion_from_euler( (float3) { 0, pickup_rotaion_speed * degrees_to_radians, 0 });
    zox_prefab_child(prefab);
    zox_prefab_name("pickup");
    zox_add(e, Pickup);
    zox_prefab_set(e, PickedUp, { pickup_state_none });
    zox_prefab_set(e, EternalRotation, { rotation_speed });
    zox_prefab_set(e, OverlapEvent, { &on_overlap_pickup });
    // collider
    zox_add(e, SphereCollider);
    zox_prefab_set(e, SphereRadius, { pickup_radius });
    zox_prefab_set(e, CollisionDisabled, { 0 });
    return e;
}
