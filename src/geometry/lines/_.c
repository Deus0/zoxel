
zox_tag(Line);
zoxc(LineThickness, float);
#include "lines2/_.c"
#include "lines3/_.c"

void import_lines(ecs* world) {
    zox_module(lines);
    zoxd_tag(Line);
    zoxd(LineThickness);
    zox_add_module(lines2);
    zox_add_module(lines3);
}
