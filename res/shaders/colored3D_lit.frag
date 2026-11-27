// a flat shaded mesh lit by the sun (doc/plans/poly_terrains.md, lit meshes): its face's normal from the screen
in vec4 vertex_color_frag;
in vec3 world_position;
const float GLOW = 3.0;   // a vertex's glow (its colour's alpha, doc/plans/hdr.md), not lit: up to this much past white
uniform float brightness;
uniform float alpha;
uniform vec3 lit_sun_direction; // normalized, towards the sun (its own, till it reads the realm's sun: doc/todo.md)
uniform vec3 sun_light;
uniform vec3 ambient_light;
#ifdef zox_outlines
layout(location=0) out vec4 frag_color;
layout(location=1) out uvec2 frag_mask;
uniform float outline;
#else
out vec4 frag_color;
#endif

void main() {
    // one face a triangle: the normal is the same across it. Turned to the camera, so a flipped y (vulkan) can't flip it
    vec3 normal = normalize(cross(dFdx(world_position), dFdy(world_position)));
    if (dot(normal, camera_position.xyz - world_position) < 0.0) {
        normal = -normal;
    }
    vec3 light = (ambient_light + sun_light * max(0.0, dot(normal, lit_sun_direction))) * sky_tint.xyz;
    vec3 base = vertex_color_frag.rgb;
    frag_color = vec4(zox_fog((base * light + base * vertex_color_frag.a * GLOW) * brightness), alpha);
#ifdef zox_outlines
    frag_mask = uvec2(uint(outline * 255.0 + 0.5), floatBitsToUint(gl_FragCoord.z));
#endif
}
