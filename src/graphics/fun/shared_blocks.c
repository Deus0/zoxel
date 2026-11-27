// a program's shared blocks (dat/shared_blocks.c) read their buffers: a block it doesn't have is skipped
void zox_gpu_bind_shared_blocks(guint program) {
    for (byte i = 0; i < zox_gpu_shared_blocks_count; i++) {
        zox_gpu_ubo_block_binding(program, zox_gpu_get_material_uniform(program, zox_gpu_shared_block_names[i]),
            zox_gpu_shared_block_bindings[i]);
    }
}
