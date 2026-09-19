entity prefab_position3;
entity prefab_position3_rotation;

entity spawn_prefab_position3(ecs *world) {
    zox_prefab();
    zox_prefab_name("position3");
    zox_setv(e, Position3D, float3_zero);
    return e;
}

entity spawn_prefab_position3_rotation(ecs *world) {
    zox_prefab();
    zox_prefab_name("position3_rotation");
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, quaternion_identity);
    return e;
}


void zox_prefabs_transforms3(ecs* world) {
    prefab_position3 = spawn_prefab_position3(world);
    prefab_position3_rotation = spawn_prefab_position3_rotation(world);
}