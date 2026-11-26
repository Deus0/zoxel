// TODO: If Camera Distance > 500, use double precision for planes - it still flickers so we need to use for now
byte is_camera_positive_z = 1;
// XR is on: a headset (zox_xr, set by xr's import) or, later, the desktop simulator. Branch on this, not #ifdef zox_xr
byte zox_xr_active = 0;
/*#if defined(zox_web)
    byte zox_disable_post_processing = 1;
#else
    byte zox_disable_post_processing = 0;
#endif*/
byte zox_disable_post_processing = 0;
// setting "Camera Outlines", --outlines / --nooutlines: outlined models on render cameras (doc/plans/outlines.md)
byte zox_camera_outlines = 1;
// setting "Tonemap", --tonemap <n>: how a render camera's colours past 1 come back to the screen's, 0 clamp (as
//  before hdr), 1 soft shoulder, 2 filmic (doc/plans/hdr.md, res/shaders/render_texture_rgb.frag tone())
byte zox_tonemap = 0;
// the camera drawing now writes the outline mask (its fbo has one, outlines on and supported), set per camera
byte render_outlines = 0;

#include "sta/_.c"
#include "dat/_.c"
#include "set/_.c"
#include "com/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "cameras2/_.c"
#include "cameras3/_.c"
#include "fun/player.c"
#include "dbg/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_cameras(ecs* world) {
    zox_module(cameras);
    zox_components_cameras(world);
    zox_systems_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_cameras);
    zox_add_module(cameras2);
    zox_add_module(cameras3);
#ifdef zox_tests
    zox_tsts_cameras();
#endif
}
