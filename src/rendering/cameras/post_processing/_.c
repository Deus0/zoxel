/*
 *  Post Processing: a render camera's view drawn on its canvas through one pass (res/shaders/render_texture_rgb.frag)
 *
 *      - effects: blur, vignette, under water tint, eyelids (a camera's values, com), outlines, the tonemap, bloom; the
 *        pass reads them from its render texture's camera as it draws (fun/pass.c)
 *      - its colour target: half floats while a tonemap is on (doc/plans/hdr.md), else bytes
 *      - settings Camera Outlines, Tonemap and Bloom (--outlines / --nooutlines, --tonemap <n>, --bloom / --nobloom)
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
}
