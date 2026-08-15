entity spawn_prefab_character3_player(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("character3_player");
    zox_add(e, PlayerCharacter);
    // Transform
    zox_prefab_set(e, Euler, { float3_zero });
    // disable until loaded terrain
    // zox_add(e, DisableMovement);
    zox_prefab_set(e, FlyMode, { 0 });
    zox_add(e, BlockSoundTrigger);
    // Controller
    zox_prefab_set(e, Movement3, { float3_zero });
    zox_prefab_set(e, CameraLink, { 0 });
    zox_prefab_set(e, PlayerLink, { 0 });
    // Raycasting
    zox_prefab_set(e, GizmoLink, { 0 });
    // Input Triggers
    zox_prefab_set(e, TriggerActionB, { 0 });
    zox_prefab_set(e, TriggerActionE, { 0 });
    // Saves
    zox_add(e, Saver);
    zox_prefab_set(e, SaveHash, { 0 });
    // Pickups
#ifdef zoxm_pickups
    const float pickup_radius = 1.5f; // 0.16f
    zox_add(e, PickUpperer);
    zox_add(e, SphereCollider);
    zox_prefab_set(e, SphereRadius, { pickup_radius });
    zox_prefab_set(e, CollisionDisabled, { 0 });
#endif
    // Debug
    if (auto_player) {
        zox_add(e, Npc);
        zox_add(e, Jumper);
        zox_prefab_set(e, Behaviour, { zox_behaviour_idle });
        zox_prefab_set(e, DefaultBehaviour, { zox_behaviour_wander });
        zox_prefab_set(e, MoveForwards, { 0 });
        zox_prefab_set(e, RotateTowards, { 0 });
        zox_prefab_set(e, MoveSpeed, { 4 });
        zox_prefab_set(e, TargetPosition, { float3_zero });
        zox_prefab_set(e, ThreatPosition, { float3_zero });
        zox_prefab_set(e, WanderDirection, { float3_zero });
    }
    // Spawn Actionbar slots
    entity actionbar = spawn_slots(world, e, 10);
    zox_set_unique_name(actionbar, "actionbar");
    zox_add(actionbar, Actionbar);
    // Spawn inventory slots
    entity inventory = spawn_slots(world, e, 16);
    zox_set_unique_name(inventory, "inventory");
    zox_add(inventory, Inventory);
    // Spawn Skillbook slots
    entity skillbook = spawn_slots(world, e, 16);
    zox_set_unique_name(skillbook, "skillbook");
    zox_add(skillbook, Skillbook);
    // Spawn Crafting Slots
    entity craftspace = spawn_slots(world, e, 10);
    zox_set_unique_name(craftspace, "craftspace");
    zox_add(craftspace, Craftspace);
    return e;
}
