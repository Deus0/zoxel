layout(location=0) in vec3 vertex_position;
uniform mat4 mvp_matrix;
uniform vec4 camera_rotation;   // the skybox turns with its camera, this turns its directions back into the world
out float fog_level;
out vec3 mesh_pos;
out vec3 sky_dir;

// a quaternion (xyz, w) rotating a vector
vec3 rotate(vec4 q, vec3 v) {
    vec3 t = 2.0 * cross(q.xyz, v);
    return v + q.w * t + cross(q.xyz, t);
}

void main() {
    gl_Position = mvp_matrix * vec4(vertex_position, 1);
    fog_level = gl_Position.z * gl_Position.z;
    mesh_pos = vertex_position;
    sky_dir = rotate(camera_rotation, vertex_position);
}
