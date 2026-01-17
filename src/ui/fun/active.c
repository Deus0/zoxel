void prefab_add_active_state(ecs* world, entity e, color base, color active) {
    zox_prefab_set(e, ActiveState, { 0 });
    zox_prefab_set(e, ActiveStateDirty, { 0 });
    // zox_prefab_set(e, OutlineColor, { base });
    zox_prefab_set(e, ElementColor, { base });
    zox_prefab_set(e, ActiveColor, { active });
}