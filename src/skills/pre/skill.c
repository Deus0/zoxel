entity spawn_prefab_skill(ecs* world) {
    zox_prefab();
    zox_prefab_name("skill");
    zox_add(e, Skill);
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, Color, { color_white });
    // zox_prefab_set(e, TextureLink, { 0 });
    zox_prefab_set(e, Activate, { 0 });
    zox_prefab_set(e, ActivateBegin, { 0 });
    zox_prefab_set(e, SkillActive, { 0 });
    zox_prefab_addc_user_timings(world, e);
    return e;
}
