// setting "Camera Outlines", --outlines / --nooutlines: outlined models on render cameras (doc/plans/outlines.md)
byte zox_camera_outlines = 1;
// setting "Tonemap", --tonemap <n>: how a render camera's colours past 1 come back to the screen's, 0 clamp (as
//  before hdr), 1 soft shoulder, 2 filmic (doc/plans/hdr.md, res/shaders/render_texture_rgb.frag tone())
byte zox_tonemap = 0;
// setting "Bloom", --bloom / --nobloom: colours past white glow into their surroundings, what's past 1 in the view's
//  mips added by zox_bloom_strength (doc/plans/hdr.md); colours under white add nothing
byte zox_bloom = 0;
float zox_bloom_strength = 1.0f;
