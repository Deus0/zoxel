zoxc(BoneIndexGPULink, uint);

ECS_DTOR(BoneIndexGPULink, ptr, {
    zox_gpu_dispose_buffer(ptr->value);
})

