// uniform blocks every program can read, by name and binding point (rendering's RenderView, the instance matrices):
//  bound as each program is made (spawn_gpu_material_program, zox_gpu_draw_program), a gpu restore's too, so no
//  material binds its own (doc/howto/shaders.md)
#define zox_gpu_shared_blocks_max 4
const char* zox_gpu_shared_block_names[zox_gpu_shared_blocks_max];
guint zox_gpu_shared_block_bindings[zox_gpu_shared_blocks_max];
byte zox_gpu_shared_blocks_count = 0;

// before any program is made: at a module's import
void zox_gpu_share_block(const char* name, guint binding) {
    if (zox_gpu_shared_blocks_count >= zox_gpu_shared_blocks_max) {
        zox_loge("[zox_gpu_share_block] no room for [%s]", name);
        return;
    }
    zox_gpu_shared_block_names[zox_gpu_shared_blocks_count] = name;
    zox_gpu_shared_block_bindings[zox_gpu_shared_blocks_count++] = binding;
}

void zox_gpu_bind_shared_blocks(guint program);   // fun/shared_blocks.c, after the backend's block functions
