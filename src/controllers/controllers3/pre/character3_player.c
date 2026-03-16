entity spawn_prefab_character3_player(ecs *world, entity p) {
    if (!p) {
        return 0;
    }

    zox_prefab_child(p);
    zox_prefab_name("character3_player");
    zox_add_tag(e, PlayerCharacter);

    // disable until loaded terrain
    zox_set(e, DisableGravity, { 1 });
    zox_set(e, DisableMovement, { 1 });
    zox_prefab_set(e, FlyMode, { 0 });
    zox_add_tag(e, BlockSoundTrigger);

    // Controller
    zox_prefab_set(e, Movement3, { float3_zero });
    zox_prefab_set(e, CameraLink, { 0 });
    zox_prefab_set(e, PlayerLink, { 0 });
    // zox_prefab_set(e, DisableMovement, { 0 });

    // Raycasting
    zox_prefab_set(e, RaycastVoxelData, { 0 });
    zox_prefab_set(e, RaycastRange, { raycaster_range });
    zox_prefab_set(e, GizmoLink, { 0 });

    // Input Triggers
    zox_prefab_set(e, TriggerActionA, { 0 });
    zox_prefab_set(e, TriggerActionB, { 0 });
    zox_prefab_set(e, TriggerActionE, { 0 });

    // IO
    zox_add_tag(e, SaveCharacter);
    zox_prefab_set(e, CharacterSaveHash, { 0 });

    // Pickups
#ifdef zoxm_pickups
    const float pickup_radius = 1.5f; // 0.16f
    zox_add_tag(e, PickUpperer);
    zox_add_tag(e, SphereCollider);
    zox_prefab_set(e, SphereRadius, { pickup_radius });
    zox_prefab_set(e, CollisionDisabled, { 0 });
#endif

    // Debug
    if (auto_player) {
        zox_add_tag(e, Npc);
        zox_add_tag(e, Jumper);
        zox_prefab_set(e, Behaviour, { zox_behaviour_idle });
        zox_prefab_set(e, DefaultBehaviour, { zox_behaviour_wander });
        zox_prefab_set(e, MoveForwards, { 0 });
        zox_prefab_set(e, RotateTowards, { 0 });
        zox_prefab_set(e, MoveSpeed, { 4 });
        zox_prefab_set(e, TargetPosition, { float3_zero });
        zox_prefab_set(e, ThreatPosition, { float3_zero });
        zox_prefab_set(e, WanderDirection, { float3_zero });
    }
    return e;
}
