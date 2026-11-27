out vec4 frag_color;
in vec3 local_position;
uniform vec4 color;

void main() {
    // shaded by face, like blocks: tops brightest, sides a little darker, bottoms darkest
    vec3 normal = cross(dFdx(local_position), dFdy(local_position));
    float shade = 0.9;
    if (abs(normal.y) > abs(normal.x) && abs(normal.y) > abs(normal.z)) {
        shade = local_position.y > 0.0 ? 1.0 : 0.8;
    }
    frag_color = vec4(zox_fog(color.xyz * shade * sky_tint.xyz), color.w);
}
