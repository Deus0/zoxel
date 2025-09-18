#ifndef zoxm_lines
#define zoxm_lines

zox_tag(Line);
zoxc(LineThickness, float);
// #include "lines2/_.c"
#include "lines3/_.c"

zox_begin_module(Lines)
    zoxd_tag(Line);
    zoxd(LineThickness);
    // zox_import_module(Lines2D)
    zox_import_module(Lines3);
zox_end_module(Lines)

#endif
