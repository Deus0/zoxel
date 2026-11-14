// GPU meshing, proof of concept (doc/tec/gpu_meshing.md): a voxel octree to a mesh in a compute shader
//  - the octree is flattened (fun/flatten.c) and uploaded, one invocation per voxel walks it like walk_octree
//  - each solid voxel's face next to air (or the chunk's edge, no neighbours yet) claims a slot by adding 6 to the
//      index count of an indirect draw command, and writes its 4 vertices and 6 indices, unordered
//  - max_faces of 3 a voxel can't overflow: a chunk's most faces are its checkers, half its voxels showing all 6
//  - vertices are vec4: xyz the corner in voxels at the meshed depth, w packs (gpu_mesh_vertex_*):
//      voxel (bits 0-7), direction (8-10), the corner's light (11-18), the corner's side of the face (19-20,
//      bit 19 its +a side, bit 20 its +b side, a and b the face's axes as smooth_face_axes)
//  - lit (gpu_mesh_flat_lit): a flat light octree too (max values, flatten_LightNode), each corner's light by
//      smooth lighting's rule (smooth_face_lights): the face's, two edges' and the corner's lights averaged, a corner
//      behind two solid edges is darklight, past the chunk sunlight above and darklight elsewhere
//  - each vertex also gets its face's 4 corner lights in one uint (byte s = corner side s), so a fragment can blend
//      them across the face (gpu_mesh_light_pixel) instead of across its two triangles
//  - the vertex, light and index buffers are plain gl buffers, the command an indirect draw's: gpu_mesher_draw draws
//      the mesh with no copy back to the cpu (tst gpu_mesh_views, gpu_light_views)
//  - meshing at a lower depth than the octree's is a lod, as getv_VoxelNode reads it (a node's value)
//  - no textures, no neighbour chunks, no merging of faces yet
//  - needs compute (OpenGL ES 3.1+, OpenGL 4.3+): gpu_mesher_create returns 0 without it, cpu meshing stays

typedef struct {
    uint program;
    uint nodes;         // flat octree (binding 0)
    uint command;       // DrawElementsIndirectCommand, its index count 6 a face (binding 1)
    uint vertices;      // 4 vec4 a face (binding 2)
    uint indices;       // 6 uints a face (binding 3)
    uint lights;        // flat light octree (binding 4)
    uint vertex_lights; // a uint a vertex: its face's 4 corner lights (binding 5)
    uint max_faces;
    byte edge_faces;    // 1: faces on the chunk's edge are drawn, 0: culled except the top (as cpu sides do with
                        //  no neighbour chunk, build_voxel_sides), 1 by default
    size_t nodes_size;  // bytes the nodes buffer holds
    size_t lights_size;
    uint vao;           // drawing the vertices and indices, made on the first draw
} GpuMesher;

// lighting for gpu_mesh_flat_lit, as smooth lighting has it (lights3)
typedef struct {
    byte darklight;     // the least light, solids and dark corners
    byte sunlight;      // past the chunk's top
    uint solid[8];      // voxel v stops light when bit v - 1 is set (the realm's solidity)
} GpuMeshLighting;

// how gpu_mesher_draw lights a mesh
#define gpu_mesh_light_none 0       // each direction a fixed shade
#define gpu_mesh_light_face 1       // a face's corners averaged, flat
#define gpu_mesh_light_vertex 2     // each corner its light, blended across the face's two triangles (as cpu meshes)
#define gpu_mesh_light_pixel 3      // the 4 corner lights blended across the whole face (bilinear, no diagonal seam)

// what a vertex's w packs
static inline byte gpu_mesh_vertex_voxel(int w) { return w & 255; }
static inline byte gpu_mesh_vertex_direction(int w) { return (w >> 8) & 7; }
static inline byte gpu_mesh_vertex_light(int w) { return (w >> 11) & 255; }
static inline byte gpu_mesh_vertex_side(int w) { return (w >> 19) & 3; }

static const uint zox_gpu_mesh_command_reset[5] = { 0, 1, 0, 0, 0 };   // count, instances, first, base vertex, base instance

#define zox_gpu_mesh_group_size 4   // invocations a group, per axis

static const char* zox_gpu_mesh_source =
"layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;\n"
"layout(std430, binding = 0) readonly buffer Nodes { uint nodes[]; };\n"
"layout(std430, binding = 1) buffer Command { uint index_count; uint instance_count; uint first_index; uint base_vertex; uint base_instance; };\n"
"layout(std430, binding = 2) writeonly buffer Vertices { vec4 vertices[]; };\n"
"layout(std430, binding = 3) writeonly buffer Indices { uint indices[]; };\n"
"layout(std430, binding = 4) readonly buffer Lights { uint lights[]; };\n"
"layout(std430, binding = 5) writeonly buffer VertexLights { uint vertex_lights[]; };\n"
"uniform int depth;\n"
"uniform uint max_faces;\n"
"uniform int lit;\n"
"uniform int edge_faces;\n"
"uniform uint darklight;\n"
"uniform uint sunlight;\n"
"uniform uint solid[8];\n"
"\n"
"bool outside(ivec3 p) {\n"
"    return any(lessThan(p, ivec3(0))) || any(greaterThanEqual(p, ivec3(1 << depth)));\n"
"}\n"
"\n"
"// the voxel at p, at depth, as walk_octree reads it: a node without children is its whole cube\n"
"uint voxel_at(ivec3 p) {\n"
"    if (outside(p)) {\n"
"        return 0u;\n"
"    }\n"
"    uint node = 0u;\n"
"    for (int d = 0; d < depth; d++) {\n"
"        uint children = nodes[node] >> 8;\n"
"        if (children == 0u) {\n"
"            break;\n"
"        }\n"
"        int shift = depth - d - 1;\n"
"        uint index = uint((p.x >> shift) & 1) | (uint((p.y >> shift) & 1) << 1) | (uint((p.z >> shift) & 1) << 2);\n"
"        node = children + index;\n"
"    }\n"
"    return nodes[node] & 255u;\n"
"}\n"
"\n"
"// the light at p, an open node holds its leaves' max (a coarse read, getv_nearby_max_LightNode)\n"
"uint light_at(ivec3 p, uint past) {\n"
"    if (outside(p)) {\n"
"        return past;\n"
"    }\n"
"    uint node = 0u;\n"
"    for (int d = 0; d < depth; d++) {\n"
"        uint children = lights[node] >> 8;\n"
"        if (children == 0u) {\n"
"            break;\n"
"        }\n"
"        int shift = depth - d - 1;\n"
"        uint index = uint((p.x >> shift) & 1) | (uint((p.y >> shift) & 1) << 1) | (uint((p.z >> shift) & 1) << 2);\n"
"        node = children + index;\n"
"    }\n"
"    return lights[node] & 255u;\n"
"}\n"
"\n"
"// stops light: a voxel the realm calls solid (past the chunk is air)\n"
"bool solid_at(ivec3 p) {\n"
"    uint v = voxel_at(p);\n"
"    if (v == 0u) {\n"
"        return false;\n"
"    }\n"
"    uint i = v - 1u;\n"
"    return ((solid[i >> 5] >> (i & 31u)) & 1u) != 0u;\n"
"}\n"
"\n"
"// left right down up back front (direction_left..direction_front)\n"
"const ivec3 directions[6] = ivec3[6](ivec3(-1, 0, 0), ivec3(1, 0, 0), ivec3(0, -1, 0), ivec3(0, 1, 0), ivec3(0, 0, -1), ivec3(0, 0, 1));\n"
"// a face's two axes along it (smooth_face_axes)\n"
"const int axis_a[6] = int[6](1, 1, 0, 0, 0, 0);\n"
"const int axis_b[6] = int[6](2, 2, 2, 2, 1, 1);\n"
"// each face's corners, counter clockwise seen from outside\n"
"const vec3 corners[24] = vec3[24](\n"
"    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), vec3(0, 1, 0),\n"
"    vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1), vec3(1, 0, 1),\n"
"    vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1),\n"
"    vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), vec3(1, 1, 0),\n"
"    vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0), vec3(1, 0, 0),\n"
"    vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(0, 1, 1));\n"
"\n"
"ivec3 unit(int axis) {\n"
"    return ivec3(axis == 0 ? 1 : 0, axis == 1 ? 1 : 0, axis == 2 ? 1 : 0);\n"
"}\n"
"\n"
"// a face's 4 corner lights, byte s the corner on side s (bit 0 its +a side, bit 1 its +b side)\n"
"uint face_lights(ivec3 p, int d) {\n"
"    uint past = d == 3 ? sunlight : darklight;\n"
"    ivec3 f = p + directions[d];\n"
"    ivec3 a = unit(axis_a[d]);\n"
"    ivec3 b = unit(axis_b[d]);\n"
"    uint face_light = light_at(f, past);\n"
"    uint corners_light = 0u;\n"
"    for (int s = 0; s < 4; s++) {\n"
"        ivec3 edge_a = f + a * ((s & 1) != 0 ? 1 : -1);\n"
"        ivec3 edge_b = f + b * ((s & 2) != 0 ? 1 : -1);\n"
"        ivec3 corner = edge_a + edge_b - f;\n"
"        uint corner_light = solid_at(edge_a) && solid_at(edge_b) ? darklight : light_at(corner, past);\n"
"        uint light = (face_light + light_at(edge_a, past) + light_at(edge_b, past) + corner_light) / 4u;\n"
"        corners_light |= light << (8 * s);\n"
"    }\n"
"    return corners_light;\n"
"}\n"
"\n"
"void main() {\n"
"    ivec3 p = ivec3(gl_GlobalInvocationID);\n"
"    if (outside(p)) {\n"
"        return;\n"
"    }\n"
"    uint voxel = voxel_at(p);\n"
"    if (voxel == 0u) {\n"
"        return;\n"
"    }\n"
"    for (int d = 0; d < 6; d++) {\n"
"        ivec3 n = p + directions[d];\n"
"        if (outside(n) ? (edge_faces == 0 && d != 3) : voxel_at(n) != 0u) {\n"
"            continue;\n"
"        }\n"
"        uint face = atomicAdd(index_count, 6u) / 6u;\n"
"        if (face >= max_faces) {\n"
"            continue;\n"
"        }\n"
"        uint face_light4 = lit != 0 ? face_lights(p, d) : 0xFFFFFFFFu;\n"
"        uint v = face * 4u;\n"
"        for (int c = 0; c < 4; c++) {\n"
"            vec3 corner = corners[d * 4 + c];\n"
"            int s = (corner[axis_a[d]] > 0.5 ? 1 : 0) | (corner[axis_b[d]] > 0.5 ? 2 : 0);\n"
"            uint light = (face_light4 >> (8 * s)) & 255u;\n"
"            float w = float(voxel) + float(d) * 256.0 + float(light) * 2048.0 + float(s) * 524288.0;\n"
"            vertices[v + uint(c)] = vec4(vec3(p) + corner, w);\n"
"            vertex_lights[v + uint(c)] = face_light4;\n"
"        }\n"
"        uint i = face * 6u;\n"
"        indices[i] = v;\n"
"        indices[i + 1u] = v + 1u;\n"
"        indices[i + 2u] = v + 2u;\n"
"        indices[i + 3u] = v;\n"
"        indices[i + 4u] = v + 2u;\n"
"        indices[i + 5u] = v + 3u;\n"
"    }\n"
"}\n";

// drawing a gpu mesh: colours from the model's palette, each direction a shade, lit by light_mode (gpu_mesh_light_*)
#define zox_gpu_mesh_colors_max 64
static const char* zox_gpu_mesh_vertex_source =
"layout(location = 0) in vec4 vertex;\n"
"layout(location = 1) in uint face_lights;\n"
"uniform mat4 camera_matrix;\n"
"uniform vec4 transform;\n"
"uniform vec4 colors[64];\n"
"out vec3 base;\n"
"out float vertex_light;\n"
"out vec2 side;\n"
"flat out vec4 corner_lights;\n"
"const float shades[6] = float[6](0.72, 0.86, 0.5, 1.0, 0.64, 0.92);\n"
"void main() {\n"
"    gl_Position = camera_matrix * vec4(transform.xyz + vertex.xyz * transform.w, 1.0);\n"
"    int w = int(vertex.w + 0.5);\n"
"    int direction = (w >> 8) & 7;\n"
"    int s = (w >> 19) & 3;\n"
"    base = colors[((w & 255) + 63) % 64].rgb * shades[direction];\n"
"    vertex_light = float((w >> 11) & 255) / 255.0;\n"
"    side = vec2(float(s & 1), float(s >> 1));\n"
"    corner_lights = vec4(float(face_lights & 255u), float((face_lights >> 8) & 255u),\n"
"        float((face_lights >> 16) & 255u), float(face_lights >> 24)) / 255.0;\n"
"}\n";
static const char* zox_gpu_mesh_fragment_source =
"in vec3 base;\n"
"in float vertex_light;\n"
"in vec2 side;\n"
"flat in vec4 corner_lights;\n"
"uniform int light_mode;\n"
"out vec4 frag_color;\n"
"void main() {\n"
"    float light = 1.0;\n"
"    if (light_mode == 1) {\n"
"        light = dot(corner_lights, vec4(0.25));\n"
"    } else if (light_mode == 2) {\n"
"        light = vertex_light;\n"
"    } else if (light_mode == 3) {\n"
"        light = mix(mix(corner_lights.x, corner_lights.y, side.x), mix(corner_lights.z, corner_lights.w, side.x), side.y);\n"
"    }\n"
"    frag_color = vec4(base * light, 1.0);\n"
"}\n";

#if defined(zox_opengl) && !defined(zox_disable_compute)

// a mesher with room for max_faces faces, 1 on success (needs a compute capable context)
byte gpu_mesher_create(GpuMesher* mesher, uint max_faces) {
    memset(mesher, 0, sizeof(GpuMesher));
    if (!zox_gpu_compute_supported()) {
        return 0;
    }
    mesher->program = zox_gpu_compute_program(zox_gpu_mesh_source);
    if (!mesher->program) {
        return 0;
    }
    const uint zero = 0;
    mesher->max_faces = max_faces;
    mesher->edge_faces = 1;
    mesher->nodes_size = sizeof(uint);
    mesher->lights_size = sizeof(uint);
    mesher->nodes = zox_gpu_storage_buffer(mesher->nodes_size, &zero);
    mesher->lights = zox_gpu_storage_buffer(mesher->lights_size, &zero);
    mesher->command = zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    mesher->vertices = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(float4), NULL);
    mesher->vertex_lights = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(uint), NULL);
    mesher->indices = zox_gpu_storage_buffer((size_t) max_faces * 6 * sizeof(uint), NULL);
    return 1;
}

void gpu_mesher_dispose(GpuMesher* mesher) {
    if (mesher->program) {
        zox_gpu_dispose_program(mesher->program);
        zox_gpu_dispose_buffer(mesher->nodes);
        zox_gpu_dispose_buffer(mesher->lights);
        zox_gpu_dispose_buffer(mesher->command);
        if (mesher->vao) {
            zox_gpu_dispose_vao(mesher->vao);
        }
        zox_gpu_dispose_buffer(mesher->vertices);
        zox_gpu_dispose_buffer(mesher->vertex_lights);
        zox_gpu_dispose_buffer(mesher->indices);
    }
    memset(mesher, 0, sizeof(GpuMesher));
}

// writes nodes into a storage buffer, growing it if needed
static inline void gpu_mesher_upload(uint buffer, size_t* buffer_size, const uint* nodes, uint count) {
    const size_t size = (size_t) count * sizeof(uint);
    if (size > *buffer_size) {
        zox_gpu_storage_upload(buffer, size, nodes);
        *buffer_size = size;
    } else {
        zox_gpu_storage_write(buffer, 0, size, nodes);
    }
}

// uploads flat nodes (fun/flatten.c) and meshes them at depth, the mesh stays on the gpu
//  - light_nodes (flatten_LightNode) and lighting light it, NULL leaves it unlit (full light)
void gpu_mesh_flat_lit(GpuMesher* mesher, const uint* nodes, uint count, const uint* light_nodes, uint light_count, byte depth, const GpuMeshLighting* lighting) {
    gpu_mesher_upload(mesher->nodes, &mesher->nodes_size, nodes, count);
    const byte lit = light_nodes && light_count && lighting;
    if (lit) {
        gpu_mesher_upload(mesher->lights, &mesher->lights_size, light_nodes, light_count);
    }
    zox_gpu_storage_write(mesher->command, 0, sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    zox_gpu_compute_use(mesher->program);
    zox_gpu_compute_uniform_int(mesher->program, "depth", depth);
    zox_gpu_compute_uniform_uint(mesher->program, "max_faces", mesher->max_faces);
    zox_gpu_compute_uniform_int(mesher->program, "lit", lit);
    zox_gpu_compute_uniform_int(mesher->program, "edge_faces", mesher->edge_faces);
    if (lit) {
        zox_gpu_compute_uniform_uint(mesher->program, "darklight", lighting->darklight);
        zox_gpu_compute_uniform_uint(mesher->program, "sunlight", lighting->sunlight);
        zox_gpu_compute_uniform_uint_array(mesher->program, "solid", lighting->solid, 8);
    }
    zox_gpu_storage_bind(0, mesher->nodes);
    zox_gpu_storage_bind(1, mesher->command);
    zox_gpu_storage_bind(2, mesher->vertices);
    zox_gpu_storage_bind(3, mesher->indices);
    zox_gpu_storage_bind(4, mesher->lights);
    zox_gpu_storage_bind(5, mesher->vertex_lights);
    const uint length = octree_size(depth);
    const uint groups = (length + zox_gpu_mesh_group_size - 1) / zox_gpu_mesh_group_size;
    zox_gpu_compute_dispatch(groups, groups, groups);
    zox_gpu_compute_use(0);
}

void gpu_mesh_flat(GpuMesher* mesher, const uint* nodes, uint count, byte depth) {
    gpu_mesh_flat_lit(mesher, nodes, count, NULL, 0, depth, NULL);
}

// flattens and meshes a voxel octree at depth (its own, or lower for a lod), returns 1 on success
byte gpu_mesh_VoxelNode(GpuMesher* mesher, const VoxelNode* voxels, byte depth) {
    uint* nodes = NULL;
    const uint count = flatten_VoxelNode(voxels, &nodes);
    if (!count) {
        return 0;
    }
    gpu_mesh_flat(mesher, nodes, count, depth);
    free(nodes);
    return 1;
}

// the faces the last mesh wrote (it waits for the gpu), at most max_faces
uint gpu_mesher_read_faces(GpuMesher* mesher) {
    uint indices = 0;
    zox_gpu_storage_read(mesher->command, 0, sizeof(uint), &indices);
    const uint faces = indices / 6;
    return faces < mesher->max_faces ? faces : mesher->max_faces;
}

// the draw program for gpu meshes, 0 without compute (dispose with zox_gpu_dispose_program)
uint gpu_mesh_draw_program() {
    if (!zox_gpu_compute_supported()) {
        return 0;
    }
    return zox_gpu_draw_program(zox_gpu_mesh_vertex_source, zox_gpu_mesh_fragment_source);
}

// draws the mesher's last mesh straight from its gpu buffers (call in a 3D render pass)
//  - position: the mesh's corner in the world, scale: a voxel's size, colors: the voxels' (voxel 1 is colors[0])
//  - light_mode: gpu_mesh_light_* (none for an unlit mesh)
void gpu_mesher_draw(GpuMesher* mesher, uint program, float4x4 camera_matrix, float3 position, float scale, const color_rgb* colors, int colors_count, byte light_mode) {
    if (!mesher->program || !program) {
        return;
    }
    if (!mesher->vao) {
        mesher->vao = zox_gpu_storage_mesh_vao(mesher->vertices, mesher->indices, 0, mesher->vertex_lights, 1);
    }
    float4 palette[zox_gpu_mesh_colors_max];
    for (int i = 0; i < zox_gpu_mesh_colors_max; i++) {
        const color_rgb c = colors_count ? colors[i % colors_count] : (color_rgb) { 200, 200, 200 };
        palette[i] = (float4) { c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, 1 };
    }
    zox_gpu_compute_use(program);
    zox_gpu_program_float4x4(program, "camera_matrix", camera_matrix);
    zox_gpu_program_float4(program, "transform", (float4) { position.x, position.y, position.z, scale });
    zox_gpu_program_float4_array(program, "colors", palette, zox_gpu_mesh_colors_max);
    zox_gpu_program_int(program, "light_mode", light_mode);
    zox_gpu_draw_indirect(mesher->vao, mesher->command);
    zox_gpu_compute_use(0);
}

#else

uint gpu_mesh_draw_program() { return 0; }
void gpu_mesher_draw(GpuMesher* mesher, uint program, float4x4 camera_matrix, float3 position, float scale, const color_rgb* colors, int colors_count, byte light_mode) { }
byte gpu_mesher_create(GpuMesher* mesher, uint max_faces) {
    (void) max_faces;
    memset(mesher, 0, sizeof(GpuMesher));
    return 0;
}
void gpu_mesher_dispose(GpuMesher* mesher) { memset(mesher, 0, sizeof(GpuMesher)); }
void gpu_mesh_flat_lit(GpuMesher* mesher, const uint* nodes, uint count, const uint* light_nodes, uint light_count, byte depth, const GpuMeshLighting* lighting) { }
void gpu_mesh_flat(GpuMesher* mesher, const uint* nodes, uint count, byte depth) { }
byte gpu_mesh_VoxelNode(GpuMesher* mesher, const VoxelNode* voxels, byte depth) { return 0; }
uint gpu_mesher_read_faces(GpuMesher* mesher) { return 0; }

#endif
