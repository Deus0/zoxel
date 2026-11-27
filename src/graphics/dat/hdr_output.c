// hdr output (doc/plans/hdr.md B): the screen itself shown brighter than sdr white, vulkan only
//  - asked for before the window is made (--hdr, --hdr-canvas), what the swapchain could do set as it's made
//  - on, the frame draws into a half float canvas (rendering/cameras/post_processing/fun/output.c) and one last draw
//    encodes it into the swapchain
#define zox_hdr_output_off 0        // the swapchain's 8 bit srgb, drawn into straight (as always)
#define zox_hdr_output_hdr10 1      // a 10 bit swapchain in hdr10: pq over 10000 nits, rec.2020
#define zox_hdr_output_scrgb 2      // a half float swapchain in scrgb: linear, 1 is 80 nits, rec.709
#define zox_hdr_output_canvas 3     // the canvas and its draw over an 8 bit swapchain, passed through (a test of it)
byte zox_hdr_output_asked = zox_hdr_output_off;    // --hdr: hdr10 or scrgb, whichever the screen offers
byte zox_hdr_output = zox_hdr_output_off;          // what the swapchain does
byte zox_hdr_screen = 0;        // the window's screen is in hdr (sdl's window says): without it, HDR is greyed
float zox_hdr_headroom = 4;     // how far past sdr white the screen goes (sdl's window says, else ~800 nits over 203)
// the frame buffer draws go back to after a render texture's (zox_gpu_reset_fbo): 0 the screen, else hdr output's canvas
guint zox_gpu_default_fbo = 0;
// hdr output's frame (rendering/cameras/post_processing/fun/output.c), run inside systems already there (new main
//  thread systems moved the frame's merges, tst tooltips): begin before the first camera or canvas draws (once a
//  frame), end before the screenshot and present (app_gpu_end_system)
void (*zox_hdr_output_begin)(ecs* world) = NULL;
void (*zox_hdr_output_end)(ecs* world) = NULL;
