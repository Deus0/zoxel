precision highp float;

out vec3 frag_color;
in vec3 mesh_pos;
in float fog_level;
uniform float brightness;
uniform vec3 sky_top_color;
uniform vec3 sky_bottom_color;
uniform float time;
uniform vec4 fog_data;

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
    vec3 sky_top_color2 = sky_top_color;
    float gradient = clamp((mesh_pos.y + 0.0) * 1.0, 0.0, 1.0);
    frag_color = vec3(mix(sky_bottom_color, sky_top_color2, gradient)) * brightness;
    frag_color = sky_gradient(frag_color);
    frag_color -= vec3(1) * 0.05;
    float noise = rand(time * mesh_pos.xy);
    frag_color += vec3(noise) * 0.1;
}
