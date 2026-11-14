// GPU meshing, proof of concept (doc/tec/gpu_meshing.md): a voxel octree to a mesh in a compute shader
//  - the octree is flattened (fun/flatten.c) and uploaded, one invocation per voxel walks it like walk_octree
//  - each solid voxel's face next to air (or the chunk's edge, no neighbours yet) claims a slot by adding 6 to the
//      index count of an indirect draw command, and writes its 4 vertices and 6 indices, unordered
//  - max_faces of 3 a voxel can't overflow: a chunk's most faces are its checkers, half its voxels showing all 6
//  - vertices are vec4: xyz the corner in voxels at the meshed depth, w = voxel + direction * 256 (the colour and
//      normal for a shader, and how tst gpu_mesh checks each face)
//  - the vertex and index buffers are plain gl buffers, the command an indirect draw's: gpu_mesher_draw draws the
//      mesh with no copy back to the cpu (tst gpu_mesh_views)
//  - meshing at a lower depth than the octree's is a lod, as getv_VoxelNode reads it (a node's value)
//  - no lights, no textures, no neighbour chunks, no merging of faces yet
//  - needs compute (OpenGL ES 3.1+, OpenGL 4.3+): gpu_mesher_create returns 0 without it, cpu meshing stays

typedef struct {
    uint program;
    uint nodes;         // flat octree (binding 0)
    uint command;       // DrawElementsIndirectCommand, its index count 6 a face (binding 1)
    uint vertices;      // 4 vec4 a face (binding 2)
    uint indices;       // 6 uints a face (binding 3)
    uint max_faces;
    size_t nodes_size;  // bytes the nodes buffer holds
    uint vao;           // drawing the vertices and indices, made on the first draw
} GpuMesher;

static const uint zox_gpu_mesh_command_reset[5] = { 0, 1, 0, 0, 0 };   // count, instances, first, base vertex, base instance

#define zox_gpu_mesh_group_size 4   // invocations a group, per axis

static const char* zox_gpu_mesh_source =
"layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;\n"
"layout(std430, binding = 0) readonly buffer Nodes { uint nodes[]; };\n"
"layout(std430, binding = 1) buffer Command { uint index_count; uint instance_count; uint first_index; uint base_vertex; uint base_instance; };\n"
"layout(std430, binding = 2) writeonly buffer Vertices { vec4 vertices[]; };\n"
"layout(std430, binding = 3) writeonly buffer Indices { uint indices[]; };\n"
"uniform int depth;\n"
"uniform uint max_faces;\n"
"\n"
"// the voxel at p, at depth, as walk_octree reads it: a node without children is its whole cube\n"
"uint voxel_at(ivec3 p) {\n"
"    int length = 1 << depth;\n"
"    if (any(lessThan(p, ivec3(0))) || any(greaterThanEqual(p, ivec3(length)))) {\n"
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
"// left right down up back front (direction_left..direction_front)\n"
"const ivec3 directions[6] = ivec3[6](ivec3(-1, 0, 0), ivec3(1, 0, 0), ivec3(0, -1, 0), ivec3(0, 1, 0), ivec3(0, 0, -1), ivec3(0, 0, 1));\n"
"// each face's corners, counter clockwise seen from outside\n"
"const vec3 corners[24] = vec3[24](\n"
"    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), vec3(0, 1, 0),\n"
"    vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1), vec3(1, 0, 1),\n"
"    vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1),\n"
"    vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), vec3(1, 1, 0),\n"
"    vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0), vec3(1, 0, 0),\n"
"    vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(0, 1, 1));\n"
"\n"
"void main() {\n"
"    ivec3 p = ivec3(gl_GlobalInvocationID);\n"
"    if (any(greaterThanEqual(p, ivec3(1 << depth)))) {\n"
"        return;\n"
"    }\n"
"    uint voxel = voxel_at(p);\n"
"    if (voxel == 0u) {\n"
"        return;\n"
"    }\n"
"    for (int d = 0; d < 6; d++) {\n"
"        if (voxel_at(p + directions[d]) != 0u) {\n"
"            continue;\n"
"        }\n"
"        uint face = atomicAdd(index_count, 6u) / 6u;\n"
"        if (face >= max_faces) {\n"
"            continue;\n"
"        }\n"
"        float w = float(voxel) + float(d) * 256.0;\n"
"        uint v = face * 4u;\n"
"        for (int c = 0; c < 4; c++) {\n"
"            vertices[v + uint(c)] = vec4(vec3(p) + corners[d * 4 + c], w);\n"
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


// drawing a gpu mesh: no lights yet, each direction a fixed shade, colours from the model's palette
#define zox_gpu_mesh_colors_max 64
static const char* zox_gpu_mesh_vertex_source =
"layout(location = 0) in vec4 vertex;\n"
"uniform mat4 camera_matrix;\n"
"uniform vec4 transform;\n"
"uniform vec4 colors[64];\n"
"out vec4 color;\n"
"const float shades[6] = float[6](0.72, 0.86, 0.5, 1.0, 0.64, 0.92);\n"
"void main() {\n"
"    gl_Position = camera_matrix * vec4(transform.xyz + vertex.xyz * transform.w, 1.0);\n"
"    int w = int(vertex.w + 0.5);\n"
"    int voxel = w & 255;\n"
"    int direction = w >> 8;\n"
"    color = vec4(colors[(voxel + 63) % 64].rgb * shades[direction], 1.0);\n"
"}\n";
static const char* zox_gpu_mesh_fragment_source =
"in vec4 color;\n"
"out vec4 frag_color;\n"
"void main() {\n"
"    frag_color = color;\n"
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
    mesher->nodes_size = sizeof(uint);
    mesher->nodes = zox_gpu_storage_buffer(mesher->nodes_size, &zero);
    mesher->command = zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    mesher->vertices = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(float4), NULL);
    mesher->indices = zox_gpu_storage_buffer((size_t) max_faces * 6 * sizeof(uint), NULL);
    return 1;
}

void gpu_mesher_dispose(GpuMesher* mesher) {
    if (mesher->program) {
        zox_gpu_dispose_program(mesher->program);
        zox_gpu_dispose_buffer(mesher->nodes);
        zox_gpu_dispose_buffer(mesher->command);
        if (mesher->vao) {
            zox_gpu_dispose_vao(mesher->vao);
        }
        zox_gpu_dispose_buffer(mesher->vertices);
        zox_gpu_dispose_buffer(mesher->indices);
    }
    memset(mesher, 0, sizeof(GpuMesher));
}

// uploads flat nodes (fun/flatten.c) and meshes them at depth, the mesh stays on the gpu
void gpu_mesh_flat(GpuMesher* mesher, const uint* nodes, uint count, byte depth) {
    const size_t size = (size_t) count * sizeof(uint);
    if (size > mesher->nodes_size) {
        zox_gpu_storage_upload(mesher->nodes, size, nodes);
        mesher->nodes_size = size;
    } else {
        zox_gpu_storage_write(mesher->nodes, 0, size, nodes);
    }
    zox_gpu_storage_write(mesher->command, 0, sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    zox_gpu_compute_use(mesher->program);
    zox_gpu_compute_uniform_int(mesher->program, "depth", depth);
    zox_gpu_compute_uniform_uint(mesher->program, "max_faces", mesher->max_faces);
    zox_gpu_storage_bind(0, mesher->nodes);
    zox_gpu_storage_bind(1, mesher->command);
    zox_gpu_storage_bind(2, mesher->vertices);
    zox_gpu_storage_bind(3, mesher->indices);
    const uint length = octree_size(depth);
    const uint groups = (length + zox_gpu_mesh_group_size - 1) / zox_gpu_mesh_group_size;
    zox_gpu_compute_dispatch(groups, groups, groups);
    zox_gpu_compute_use(0);
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
void gpu_mesher_draw(GpuMesher* mesher, uint program, float4x4 camera_matrix, float3 position, float scale, const color_rgb* colors, int colors_count) {
    if (!mesher->program || !program) {
        return;
    }
    if (!mesher->vao) {
        mesher->vao = zox_gpu_storage_mesh_vao(mesher->vertices, mesher->indices, 0);
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
    zox_gpu_draw_indirect(mesher->vao, mesher->command);
    zox_gpu_compute_use(0);
}

#else

uint gpu_mesh_draw_program() { return 0; }
void gpu_mesher_draw(GpuMesher* mesher, uint program, float4x4 camera_matrix, float3 position, float scale, const color_rgb* colors, int colors_count) { }

byte gpu_mesher_create(GpuMesher* mesher, uint max_faces) {
    (void) max_faces;
    memset(mesher, 0, sizeof(GpuMesher));
    return 0;
}
void gpu_mesher_dispose(GpuMesher* mesher) { memset(mesher, 0, sizeof(GpuMesher)); }
void gpu_mesh_flat(GpuMesher* mesher, const uint* nodes, uint count, byte depth) { }
byte gpu_mesh_VoxelNode(GpuMesher* mesher, const VoxelNode* voxels, byte depth) { return 0; }
uint gpu_mesher_read_faces(GpuMesher* mesher) { return 0; }

#endif
