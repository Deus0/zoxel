#include "line.c"
#include "ui_line.c"
#include "temp.c"

entity prefab_line2D;
entity prefab_ui_line2D;
entity prefab_temporary_ui_line2D;
entity prefab_temporary_line2D;
// extern entity prefab_temporary_line2D;
// extern entity prefab_temporary_ui_line2D;
// extern void set_ui_line_position(LineData2D*, int4, float2);

void spawn_prefabs_lines2D(ecs *world) {
    prefab_line2D = spawn_prefab_line2D(world);
    prefab_ui_line2D = spawn_prefab_ui_line2D(world);
    prefab_temporary_line2D = spawn_prefab_temp(world, prefab_line2D);
    prefab_temporary_ui_line2D  = spawn_prefab_temp(world, prefab_ui_line2D);
}
