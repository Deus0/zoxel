// tst: lines_spawn, the 3D line spawners make lines with what they're given, and lines with a lifetime go
//  - spawns three lines using different spawn functions with varying lifetimes
//  - checks that permanent line exists with correct data and color
//  - verifies temporary lines are deleted after their lifetime
//  - run: bash bsh/tst.sh lines_spawn

static entity zox_tst_lines_line1 = 0;
static entity zox_tst_lines_line2 = 0;
static entity zox_tst_lines_line3 = 0;
static double zox_tst_lines_start_time = 0;

byte zox_tst_lines_spawn_ready(ecs* world) {
    if (zox_tst_lines_line1 == 0) {
        float3 a = (float3) { 1, 2, 3 };
        float3 b = (float3) { 4, 5, 6 };
        zox_tst_lines_line1 = spawn_line3c(world, a, b, 2.0f, 0, (color_rgb) { 255, 0, 0 });
        zox_tst_lines_line2 = spawn_line3_alpha(world, a, b, 3.0f, 0.2, (color) { 0, 255, 0, 255 });
        zox_tst_lines_line3 = spawn_line3(world, a, b, 4.0f, 0.2);
        zox_tst_lines_start_time = zox_current_time;
        return 0;
    }
    if (zox_current_time - zox_tst_lines_start_time < 0.6) {
        zox_tst_waiting = "temporary lines to expire";
        return 0;
    }
    return 1;
}

void zox_tst_lines_spawn_check(ecs* world) {
    // the permanent line keeps what it was spawned with
    zox_tst_check(zox_alive(zox_tst_lines_line1), "the permanent line was deleted");
    if (zox_alive(zox_tst_lines_line1)) {
        const float6 line = zox_getv(zox_tst_lines_line1, LineData3D);
        zox_tst_check(line.x == 1 && line.y == 2 && line.z == 3, "start [%fx%fx%f], expected [1x2x3]", line.x, line.y, line.z);
        zox_tst_check(line.w == 4 && line.u == 5 && line.v == 6, "end [%fx%fx%f], expected [4x5x6]", line.w, line.u, line.v);
        const float thickness = zox_getv(zox_tst_lines_line1, LineThickness);
        zox_tst_check(thickness == 2.0f, "thickness [%f], expected [2]", thickness);
        const color c = zox_getv(zox_tst_lines_line1, Color);
        zox_tst_check(c.r == 255 && c.g == 0 && c.b == 0 && c.a == 255, "color [%i %i %i %i], expected [255 0 0 255]", c.r, c.g, c.b, c.a);
    }

    // Check temporary lines are gone
    zox_tst_check(!zox_alive(zox_tst_lines_line2), "the line with a lifetime (spawn_line3_alpha) wasn't deleted");
    zox_tst_check(!zox_alive(zox_tst_lines_line3), "the line with a lifetime (spawn_line3) wasn't deleted");

    // Clean up
    if (zox_alive(zox_tst_lines_line1)) {
        zox_delete(zox_tst_lines_line1);
    }
}

void zox_tsts_lines3() {
    zox_tst_add("lines_spawn", zox_tst_lines_spawn_ready, zox_tst_lines_spawn_check, 30);
}

