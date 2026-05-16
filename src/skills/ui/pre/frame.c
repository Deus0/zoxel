entity spawn_prefab_frame_skill(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("frame_skill");
    zox_add_tag(e, FrameSkill);
    return e;
}
