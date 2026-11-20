// vulkan_shaders: every program's uniforms are inside its blocks, as reflected from the spir-v
//  - a block measured short drops writes past its end: its last uniforms stay 0 (2026-11-20: constants read as
//    types made floats 0 bytes, brightness 0, black skeletons and items, half a black sky, no particles)
//  - ready once the game's shaders are loaded (a few frames); checks a dozen programs at least were made

byte zox_tst_vulkan_shaders_ready(ecs* world) {
    (void) world;
    return vk_frames_presented >= 3;
}

void zox_tst_vulkan_shaders_check(ecs* world) {
    (void) world;
    int programs = 0;
    int uniforms = 0;
    for (uint id = 1; id <= vk_programs.count; id++) {
        const vk_program* program = vk_program_of(id);
        if (!program) {
            continue;
        }
        programs++;
        for (int u = 0; u < program->uniforms_count; u++) {
            const vk_uniform* uniform = &program->uniforms[u];
            for (byte s = 0; s < 2; s++) {
                if (uniform->offset[s] < 0) {
                    continue;
                }
                uniforms++;
                // at least a float past its offset
                zox_tst_check(uniform->offset[s] + 4 <= program->data_size[s],
                    "program [%u] uniform [%s] at [%i] is past its %s block's [%i] bytes", id, uniform->name,
                    uniform->offset[s], s ? "fragment" : "vertex", program->data_size[s]);
            }
        }
    }
    zox_tst_check(programs >= 12, "only [%i] programs made", programs);
    zox_log("[tst] vulkan programs [%i] uniforms [%i]", programs, uniforms);
}
