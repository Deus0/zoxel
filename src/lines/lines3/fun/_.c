#include "planes.c"
#include "frustrum.c"
#include "terrain_grid.c"
#include "line.c"
#include "debug.c"
#include "cube.c"

// NOTE: Shorthand for lines
static inline void zox_gpu_line(float3 a, float3 b) {
    const float* line = (float[]) { a.x, a.y, a.z, b.x, b.y, b.z };
    zox_gpu_set_attribute_float3(line3D_position_location, line);
    zox_gpu_render_lines(2);
}

static inline void zox_gpu_quad_lines(float3 p, float3 a, float3 b, float3 c, float3 d, float3 shrink, float3 extrude) {
    a = float3_multiply(a, shrink);
    b = float3_multiply(b, shrink);
    c = float3_multiply(c, shrink);
    d = float3_multiply(d, shrink);
    a = float3_add(p, a);
    b = float3_add(p, b);
    c = float3_add(p, c);
    d = float3_add(p, d);
    // top
    zox_gpu_line(
        float3_add(a, extrude),
        float3_add(b, extrude)
    );
    zox_gpu_line(
        float3_add(c, extrude),
        float3_add(d, extrude)
    );
    zox_gpu_line(
        float3_add(a, extrude),
        float3_add(c, extrude)
    );
    zox_gpu_line(
        float3_add(b, extrude),
        float3_add(d, extrude)
    );
}
