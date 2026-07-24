entity spawn_prefab_character3_npc(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("character3_npc");
    zox_add_tag(e, Npc);
    zox_add_tag(e, VoxMesh);
    zox_add_tag(e, Jumper);
    zox_add_tag(e, SilentJumper);   //  dont have 3d sounds yet
    zox_prefab_set(e, Behaviour, { zox_behaviour_idle });
    zox_prefab_set(e, DefaultBehaviour, { zox_behaviour_wander });
    zox_prefab_set(e, MoveForwards, { 0 });
    zox_prefab_set(e, RotateTowards, { 0 });
    zox_prefab_set(e, MoveSpeed, { 4 });
    zox_prefab_set(e, MoveToBuffer, { default_npc_follow_distance }); // 0.45f
    zox_prefab_set(e, TargetPosition, { float3_zero });
    zox_prefab_set(e, ThreatPosition, { float3_zero });
    zox_prefab_set(e, WanderDirection, { float3_zero });
    zox_set(e, RenderDisabled, { 1 });
    // Spawn Actionbar slots
    /*entity actionbar = spawn_slots(world, e, 4);
    zox_set_unique_name(actionbar, "actionbar");
    zox_add_tag(actionbar, Actionbar);*/
    return e;
}
