// A character with a skeleton child, its body is combined from parts into its vox child
entity spawn_prefab_character3_skeleton(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("character3_skeleton");
    // NOTE: Skeleton is spawned in CharacterChildrenSpawnSystem
    zox_setv(e, SkeletonLink, 0);
    // NOTE: MaxRenderDepth is set by BodyCombineSystem
    return e;
}
