#ifndef zoxm_space
#define zoxm_space

// one day we will actually make this abuot space
zox_tag(Planet);
zox_tag(Asteroid);
zox_tag(Star);
#include "fun/_.c"

zox_begin_module(Space)
    zoxd_tag(Planet);
    zoxd_tag(Asteroid);
    zoxd_tag(Star);

zox_end_module(Space)

// todo: when selecting pickaxe, you can destroy voxels, or with your fists
// todo: you can only place color cubes on block voxes, which drop from enemies
// todo: when placing new color in a Vox, add it to the color lookup there - unless max colors.. maybe just remove lowest color count if thats the case!
// todo: A merchant who sells color cubes
// todo: raycast a character as well
// todo: collisions to be per voxel, instead of per bounding box... that will be hard
// todo: rotational physics..!

#endif