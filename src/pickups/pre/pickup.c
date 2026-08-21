// based off cube prefab
entity spawn_prefab_pickup(ecs *world, entity prefab) {
    float pickup_radius = 0.15f;
    float3 rotation_euler = (float3) { 0, pickup_rotaion_speed * degrees_to_radians, 0 };
    float4 rotation_speed = quaternion_from_euler(rotation_euler);
    zox_prefab_child(prefab);
    zox_prefab_name("pickup");
    zox_add(e, Pickup);
    zox_setv(e, PickedUp, pickup_state_none);
    zox_setv(e, EternalRotation, rotation_speed);
    zox_setv(e, OverlapEvent, &on_overlap_pickup);
    // collider
    zox_add(e, SphereCollider);
    zox_setv(e, SphereRadius, pickup_radius);
    zox_setv(e, CollisionDisabled, 0);
    return e;
}
