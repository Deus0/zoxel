// Voxel data combined from a list of voxes, i.e. a skeleton characters body parts
//  - set CombineList, CombinePositions, CombineDepths then trigger CombineVox
entity spawn_prefab_vox_combined(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_combined");
    zox_setv(e, CombineVox, 0);
    zox_prefab_add(e, CombineList);
    zox_prefab_add(e, CombinePositions);
    zox_prefab_add(e, CombineDepths);
    return e;
}
