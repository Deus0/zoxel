out vec3 frag_color;
in vec3 mesh_pos;
in float fog_level;
// uniform float brightness;
uniform vec4 color;
uniform vec4 fog_data;
uniform float time;
uniform vec3 sky_top_color;
uniform vec3 sky_bottom_color;

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec3 sky_gradient(vec3 color) {
    if (mesh_pos.y > 0.1) {
        float height_power = (mesh_pos.y - 0.1) * (1.0 - abs(max(mesh_pos.x, mesh_pos.z))) * 1.1;
        color.x += height_power;
        color.y += height_power;
        color.z += height_power;
        if (mesh_pos.x >= -0.1 && mesh_pos.x <= 0.1 && mesh_pos.z >= -0.1 && mesh_pos.z <= 0.1) {
            float to_mid_power = (1.0 - 10.0 * max(abs(mesh_pos.x), abs(mesh_pos.z)));
            color.x += to_mid_power;
            color.y += to_mid_power;
            color.z += to_mid_power;
            float noise2 = rand(time * mesh_pos.xy);
            color -= vec3(noise2) * 1.4;
        }
    }
    return color;
}

void main() {
    // Main vertical sky gradient.
    // Keeps the horizon close to the terrain colour while becoming
    // substantially lighter toward the top.
    float gradient = smoothstep(0.0, 1.0, mesh_pos.y);
    frag_color = mix(
        sky_bottom_color,
        sky_top_color,
        gradient * 0.7
    );

    // Extra lift toward the upper sky so the gradient is actually visible.
    float upper_gradient = smoothstep(0.35, 1.0, mesh_pos.y);
    frag_color += vec3(upper_gradient * 0.08);

    // Existing sky/sun shaping.
    frag_color = sky_gradient(frag_color);

    // Small animated variation.
    float noise = rand(time * mesh_pos.xy);
    frag_color += vec3((noise * 0.2) - 0.1);

}
