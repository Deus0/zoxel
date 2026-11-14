// Voxel data combined from a list of voxes, i.e. a skeleton characters body parts
//  - set CombineList, CombinePositions, CombineDepths, CombineOver then trigger CombineVox (BoneNode: its voxels' parts)
entity spawn_prefab_vox_combined(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("vox_combined");
    zox_setv(e, CombineVox, 0);
    zox_add(e, CombineList);
    zox_add(e, CombinePositions);
    zox_add(e, CombineDepths);
    zox_add(e, CombineOver);
    zox_add(e, CombineBones);
    zox_setv(e, BoneNode, 0);
    return e;
}
