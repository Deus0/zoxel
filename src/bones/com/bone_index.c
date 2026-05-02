zoxc(BoneIndexGPULink, uint);

void add_gpu_bone_index(ecs *world, entity e) {
    zox_prefab_set(e, BoneIndexGPULink, { 0 });
}

void spawn_gpu_bone_index(ecs *world, entity e) {
    zox_set(e, BoneIndexGPULink, { zox_gpu_create_buffer() });
}

ECS_DTOR(BoneIndexGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})

