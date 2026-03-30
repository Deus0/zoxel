void prefab_add_active_state(ecs* world, entity e, color active) {
    zox_prefab_set(e, ActiveState, { 0 });
    zox_prefab_set(e, ActiveStateDirty, { 0 });
    // zox_prefab_set(e, OutlineColor, { base });

    zox_geter_value(e, OutlineColor, color, base);
    zox_prefab_set(e, ElementColor, { base });

    zox_prefab_set(e, ActiveColor, { active });

    zox_log("Set prefab [%s] outline color [%ix%ix%ix%i]", zox_get_name(e), base.r, base.g, base.b, base.a);
}
