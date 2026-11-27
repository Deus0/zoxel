/*
 *  Post Processing: a render camera's view drawn on its canvas through one pass (res/shaders/render_texture_rgb.frag)
 *
 *      - effects: blur, vignette, under water tint, eyelids (a camera's values, com), outlines, the tonemap, bloom; the
 *        pass reads them from its render texture's camera as it draws (fun/pass.c)
 *      - its colour target: half floats while a tonemap is on (doc/plans/hdr.md), else bytes
 *      - settings Camera Outlines, Tonemap, Bloom and HDR (--outlines / --nooutlines, --tonemap <n>, --bloom / --nobloom,
 *        --hdr / --nohdr: the screen in hdr, vulkan, fun/output.c)
 *      - its material: rendering2/mat/render_texture.c; tests: ui/render_textures/tst (eyelids, hdr_target)
 *
 * */
#include "com/_.c"
#include "set/_.c"
#include "fun/_.c"
#include "eve/_.c"
#include "pre/_.c"

void import_post_processing(ecs* world) {
    zox_module(post_processing);
    zox_components_post_processing(world);
    add_hook_spawn_prefabs(spawn_prefabs_post_processing);
    add_hook_terminal_command(process_arguments_post_processing);
    add_hook_gpu_dispose(gpu_dispose_hdr_output);
    add_hook_gpu_restore(gpu_restore_hdr_output);
    // hdr output (fun/output.c): the frame into its canvas before the cameras, encoded after everything's drawn
    zox_system_1(hdr_output_begin_system, zoxp_cameras, 0);
    zox_system_1(hdr_output_end_system, zoxp_pre_end, 0);
}
