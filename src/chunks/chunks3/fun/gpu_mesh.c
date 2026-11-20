// GPU meshing, proof of concept (doc/plans/gpu_meshing.md): a voxel octree to a mesh in a compute shader
//  - the chunk's voxels at the meshed depth as a grid of bytes with a voxel of border from its 26 neighbours
//      (smooth lighting's grid, lights3/fun/smooth_grid.c) are uploaded, one invocation per voxel reads it
//  - a face into a neighbour chunk shows when its voxel there is air, into a missing chunk as edge_faces says
//  - each solid voxel's face next to air (or the chunk's edge, no neighbours yet) claims a slot by adding 6 to the
//      index count of an indirect draw command, and writes its 4 vertices and 6 indices, unordered
//  - every face is counted (faces_wanted, its own buffer: one the draws read stalls a read back), only the first max_faces are written and drawn:
//      a mesher sized by a guess grows when the count comes back over it (gpu_mesher_counted, gpu_mesher_grow) and
//      meshes again; 3 a voxel can't overflow (a chunk's most faces are its checkers, half its voxels showing all 6)
//  - vertices are vec4: xyz the corner in voxels at the meshed depth, w packs (gpu_mesh_vertex_*):
//      voxel (bits 0-7), direction (8-10), the corner's light (11-18), the corner's side of the face (19-20,
//      bit 19 its +a side, bit 20 its +b side, a and b the face's axes as smooth_face_axes)
//  - lit: the lights grid too (an open node its leaves' max), each corner's light by smooth lighting's rule
//      (smooth_face_lights): the face's, two edges' and the corner's lights averaged, a corner behind two solid
//      edges is darklight, in a chunk without lights sunlight above and darklight elsewhere
//  - each vertex also gets its face's 4 corner lights in one uint (byte s = corner side s), so a fragment can blend
//      them across the face (gpu_mesh_light_pixel) instead of across its two triangles
//  - the vertex, light and index buffers are plain gl buffers, the command an indirect draw's: gpu_mesher_draw draws
//      the mesh with no copy back to the cpu (tst gpu_mesh_views, gpu_light_views)
//  - meshing at a lower depth than the octree's is a lod: the grid is filled at that depth (a node's value)
//  - lower blocks (flowing water): heights, 4 bits a block in eighths (0 full, gpu_mesher_set_heights), its top and
//      its sides' tops at its height when its top shows (as chunks3 build_textured.c)
//  - a chunk material's mesh (gpu_mesher_set_material): only its block material's voxels, faces hidden as cpu sides
//      hide them (block_material_hides), by two 256 bit masks
//  - textured (gpu_mesher_set_uvs): each vertex its tilemap uv, the cpu's for that block, direction and corner
//  - no merging of faces yet
//  - needs compute (OpenGL ES 3.1+, OpenGL 4.3+): gpu_mesher_create returns 0 without it, cpu meshing stays

typedef struct {
    uint program;
    uint voxels;        // the voxels grid, 4 bytes a uint (binding 0)
    uint command;       // DrawElementsIndirectCommand, its index count 6 a face (binding 1)
    uint count;         // the faces the mesh wanted, read back (binding 8), never drawn from
    uint vertices;      // 4 vec4 a face (binding 2)
    uint indices;       // 6 uints a face (binding 3)
    uint lights;        // the lights grid (binding 4)
    uint vertex_lights; // a uint a vertex: its face's 4 corner lights (binding 5)
    uint uvs;           // the tilemap's uvs, 24 a block: its 6 directions' 4 corners as the cpu's (binding 6), not its own
    uint vertex_uvs;    // a vec2 a vertex, written when uvs are given (binding 7)
    byte textured;      // uvs given (gpu_mesher_set_uvs)
    uint max_faces;     // room in the vertex, light and index buffers
    ulong fence;        // after the last mesh, 0 once its count was read (gpu_mesher_counted)
    byte edge_faces;    // faces into a missing chunk, 1: drawn, 0: culled except the top (as cpu sides do with no
                        //  neighbour chunk, build_voxel_sides), 1 by default
    uint vao;           // drawing the vertices and indices, made on the first draw
    uint heights[32];   // a block's height in eighths, 4 bits a block (voxel v at bits (v - 1) * 4), 0 full
    uint drawn[8];      // voxels it draws, bit v - 1 (a chunk material's blocks), all by default
    uint hides[8];      // voxels that hide its faces, bit v - 1 (block_material_hides), all by default
} GpuMesher;

// lighting for gpu_mesh_grid, as smooth lighting has it (lights3)
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

// the indirect draw's count, instances, first, base vertex, base instance
static const uint zox_gpu_mesh_command_reset[5] = { 0, 1, 0, 0, 0 };
static const uint zox_gpu_mesh_count_reset = 0;

#define zox_gpu_mesh_group_size 4   // invocations a group, per axis

static const char* zox_gpu_mesh_source =
"layout(local_size_x = 4, local_size_y = 4, local_size_z = 4) in;\n"
"layout(std430, binding = 0) readonly buffer Voxels { uint voxels[]; };\n"
"layout(std430, binding = 1) buffer Command { uint index_count; uint instance_count; uint first_index; uint base_vertex; uint base_instance; };\n"
"layout(std430, binding = 8) buffer Count { uint faces_wanted; };\n"
"layout(std430, binding = 2) writeonly buffer Vertices { vec4 vertices[]; };\n"
"layout(std430, binding = 3) writeonly buffer Indices { uint indices[]; };\n"
"layout(std430, binding = 4) readonly buffer Lights { uint lights[]; };\n"
"layout(std430, binding = 5) writeonly buffer VertexLights { uint vertex_lights[]; };\n"
"layout(std430, binding = 6) readonly buffer Uvs { vec2 uvs[]; };\n"
"layout(std430, binding = 7) writeonly buffer VertexUvs { vec2 vertex_uvs[]; };\n"
"uniform int depth;\n"
"uniform uint max_faces;\n"
"uniform int lit;\n"
"uniform int edge_faces;\n"
"uniform uint present;\n"
"uniform uint lights_present;\n"
"uniform uint darklight;\n"
"uniform uint sunlight;\n"
"uniform uint solid[8];\n"
"uniform uint heights[32];\n"
"uniform uint drawn[8];\n"
"uniform uint hides[8];\n"
"uniform int textured;\n"
"uniform uint face_corners[24];\n"
"\n"
"// does this mesh draw voxel v (its block material's), bit v - 1\n"
"bool is_drawn(uint v) {\n"
"    uint i = v - 1u;\n"
"    return v != 0u && ((drawn[i >> 5] >> (i & 31u)) & 1u) != 0u;\n"
"}\n"
"\n"
"// does voxel v hide this mesh's faces (block_material_hides), bit v - 1\n"
"bool hides_face(uint v) {\n"
"    uint i = v - 1u;\n"
"    return v != 0u && ((hides[i >> 5] >> (i & 31u)) & 1u) != 0u;\n"
"}\n"
"\n"

"bool outside(ivec3 p) {\n"
"    return any(lessThan(p, ivec3(0))) || any(greaterThanEqual(p, ivec3(1 << depth)));\n"
"}\n"
"\n"
"// the nearby chunk p is in (OCTREE_INDEX), p at most a voxel past the chunk\n"
"uint chunk_of(ivec3 p) {\n"
"    ivec3 s = ivec3(greaterThanEqual(p, ivec3(1 << depth))) - ivec3(lessThan(p, ivec3(0)));\n"
"    return uint((s.z + 1) * 9 + (s.y + 1) * 3 + (s.x + 1));\n"
"}\n"
"\n"
"// p's byte in the grids: x then y then z, a voxel of border\n"
"uint cell(ivec3 p) {\n"
"    int length = (1 << depth) + 2;\n"
"    return uint(((p.x + 1) * length + p.y + 1) * length + p.z + 1);\n"
"}\n"
"\n"
"// the voxel at p, a missing chunk's air\n"
"uint voxel_at(ivec3 p) {\n"
"    uint i = cell(p);\n"
"    return (voxels[i >> 2] >> ((i & 3u) * 8u)) & 255u;\n"
"}\n"
"\n"
"// is p in a chunk that isn't there\n"
"bool missing(ivec3 p) {\n"
"    return ((present >> chunk_of(p)) & 1u) == 0u;\n"
"}\n"
"\n"
"// the light at p, past (sunlight above, darklight elsewhere) in a chunk without lights\n"
"uint light_at(ivec3 p, uint past) {\n"
"    if (((lights_present >> chunk_of(p)) & 1u) == 0u) {\n"
"        return past;\n"
"    }\n"
"    uint i = cell(p);\n"
"    return (lights[i >> 2] >> ((i & 3u) * 8u)) & 255u;\n"
"}\n"
"\n"
"// stops light: a voxel the realm calls solid (a missing chunk is air)\n"
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
"// does a voxel's face that way show: into a missing chunk as edge_faces says (the top always), else unless hidden\n"
"bool face_shows(ivec3 p, int d) {\n"
"    ivec3 n = p + directions[d];\n"
"    if (outside(n) && missing(n)) {\n"
"        return edge_faces != 0 || d == 3;\n"
"    }\n"
"    return !hides_face(voxel_at(n));\n"
"}\n"
"\n""void main() {\n"
"    ivec3 p = ivec3(gl_GlobalInvocationID);\n"
"    if (outside(p)) {\n"
"        return;\n"
"    }\n"
"    uint voxel = voxel_at(p);\n"
"    if (!is_drawn(voxel)) {\n"
"        return;\n"
"    }\n"
"    // a lower block (flowing water): its top at its height, when its top shows\n"
"    uint height = (heights[(voxel - 1u) >> 3] >> (((voxel - 1u) & 7u) * 4u)) & 15u;\n"
"    bool lowered = height != 0u && face_shows(p, 3);\n"
"    for (int d = 0; d < 6; d++) {\n"
"        if (!face_shows(p, d)) {\n"
"            continue;\n"
"        }\n"
"        // every face counted, only those with room written and drawn\n"
"        uint face = atomicAdd(faces_wanted, 1u);\n"
"        if (face >= max_faces) {\n"
"            continue;\n"
"        }\n"
"        atomicAdd(index_count, 6u);\n"
"        uint face_light4 = lit != 0 ? face_lights(p, d) : 0xFFFFFFFFu;\n"
"        uint v = face * 4u;\n"
"        for (int c = 0; c < 4; c++) {\n"
"            vec3 corner = corners[d * 4 + c];\n"
"            if (textured != 0) {\n"
"                // the tilemap's uv for this corner: its block's, its direction's, the cpu's corner at it\n"
"                uint bits = uint(corner.x > 0.5) | (uint(corner.y > 0.5) << 1) | (uint(corner.z > 0.5) << 2);\n"
"                uint k = 0u;\n"
"                for (uint j = 0u; j < 4u; j++) {\n"
"                    if (face_corners[uint(d) * 4u + j] == bits) {\n"
"                        k = j;\n"
"                    }\n"
"                }\n"
"                vertex_uvs[v + uint(c)] = uvs[(voxel - 1u) * 24u + uint(d) * 4u + k];\n"
"            }\n"
"            if (lowered && corner.y > 0.5) {\n"
"                corner.y = float(height) / 8.0;\n"
"            }\n"
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

// drawing a textured gpu chunk mesh, as textured3D draws a cpu one: the tilemap's texel times its light (its face's 4
//  corner lights blended across the face, no diagonal seam), brightness and fog
//  - transform: xyz unused, w a voxel's size; shading: x brightness
static const char* zox_gpu_mesh_textured_vertex_source =
"layout(location = 0) in vec4 vertex;\n"
"layout(location = 1) in uint face_lights;\n"
"layout(location = 2) in vec2 vertex_uv;\n"
"uniform mat4 mvp_matrix;\n"
"uniform vec4 transform;\n"
"out float fog_level;\n"
"out vec2 uv;\n"
"out vec2 side;\n"
"flat out vec4 corner_lights;\n"
"void main() {\n"
"    gl_Position = mvp_matrix * vec4(vertex.xyz * transform.w, 1.0);\n"
"    fog_level = gl_Position.z * gl_Position.z;\n"
"    uv = vertex_uv;\n"
"    int s = (int(vertex.w + 0.5) >> 19) & 3;\n"
"    side = vec2(float(s & 1), float(s >> 1));\n"
"    corner_lights = vec4(float(face_lights & 255u), float((face_lights >> 8) & 255u),\n"
"        float((face_lights >> 16) & 255u), float(face_lights >> 24)) / 255.0;\n"
"}\n";
static const char* zox_gpu_mesh_textured_fragment_source =
"uniform vec4 fog_data;\n"
"uniform vec4 shading;\n"
"uniform sampler2D zexture;\n"
"in float fog_level;\n"
"in vec2 uv;\n"
"in vec2 side;\n"
"flat in vec4 corner_lights;\n"
"out vec4 frag_color;\n"
"void main() {\n"
"    vec4 texel = texture(zexture, uv);\n"
"    if (texel.a <= 0.0) {\n"
"        discard;\n"
"    }\n"
"    float light = mix(mix(corner_lights.x, corner_lights.y, side.x), mix(corner_lights.z, corner_lights.w, side.x), side.y);\n"
"    vec3 color = texel.rgb * light * shading.x;\n"
"    float fog_blend = min(1.0, 1.1 - exp2(-fog_data.w * fog_level));\n"
"    frag_color = vec4(mix(color, fog_data.xyz, fog_blend), texel.a);\n"
"}\n";

#if (defined(zox_opengl) || defined(zox_vulkan)) && !defined(zox_disable_compute)

// the textured chunk draw program, 0 without compute
uint gpu_mesh_textured_program() {
    if (!zox_gpu_compute_supported()) {
        return 0;
    }
    return zox_gpu_draw_program(zox_gpu_mesh_textured_vertex_source, zox_gpu_mesh_textured_fragment_source);
}

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
    memset(mesher->drawn, 255, sizeof(mesher->drawn));
    memset(mesher->hides, 255, sizeof(mesher->hides));
    mesher->voxels = zox_gpu_storage_buffer(sizeof(uint), &zero);
    mesher->lights = zox_gpu_storage_buffer(sizeof(uint), &zero);
    mesher->command = zox_gpu_storage_buffer(sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    mesher->count = zox_gpu_storage_buffer_read(sizeof(uint), &zox_gpu_mesh_count_reset);
    mesher->vertices = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(float4), NULL);
    mesher->vertex_lights = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(uint), NULL);
    mesher->indices = zox_gpu_storage_buffer((size_t) max_faces * 6 * sizeof(uint), NULL);
    mesher->vertex_uvs = zox_gpu_storage_buffer((size_t) max_faces * 4 * sizeof(float2), NULL);
    return 1;
}

void gpu_mesher_dispose(GpuMesher* mesher) {
    if (mesher->program) {
        zox_gpu_dispose_program(mesher->program);
        zox_gpu_dispose_buffer(mesher->voxels);
        zox_gpu_dispose_buffer(mesher->lights);
        zox_gpu_dispose_buffer(mesher->command);
        zox_gpu_dispose_buffer(mesher->count);
        if (mesher->vao) {
            zox_gpu_dispose_vao(mesher->vao);
        }
        zox_gpu_dispose_buffer(mesher->vertices);
        zox_gpu_dispose_buffer(mesher->vertex_lights);
        zox_gpu_dispose_buffer(mesher->indices);
        zox_gpu_dispose_buffer(mesher->vertex_uvs);
        zox_gpu_fence_dispose(mesher->fence);
    }
    memset(mesher, 0, sizeof(GpuMesher));
}

// a block's height in eighths, by block index - 1 (255 of them, BlockWaterLevel under 8: flowing water), 0 full
static inline void gpu_mesher_set_heights(GpuMesher* mesher, const byte* heights) {
    memset(mesher->heights, 0, sizeof(mesher->heights));
    for (int i = 0; i < 255; i++) {
        mesher->heights[i >> 3] |= (uint) (heights[i] & 15) << ((i & 7) * 4);
    }
}

// a chunk material's mesh: the voxels of block material (materials: get_block_materials' table) and what hides
//  their faces, as cpu sides (build_voxel_sides)
static inline void gpu_mesher_set_material(GpuMesher* mesher, const byte* materials, byte material) {
    memset(mesher->drawn, 0, sizeof(mesher->drawn));
    memset(mesher->hides, 0, sizeof(mesher->hides));
    for (int v = 1; v < 256; v++) {
        const uint bit = 1u << ((v - 1) & 31);
        if (materials[v] == material) {
            mesher->drawn[(v - 1) >> 5] |= bit;
        }
        if (block_material_hides(materials[v], material)) {
            mesher->hides[(v - 1) >> 5] |= bit;
        }
    }
}

// the cpu's face corners (voxel_face_vertices_n, blocks/dat/mesh.c), bit 0 x, 1 y, 2 z: a corner's uv is the
//  tilemap's at the cpu corner in the same place (uploaded once, made from the table on the first mesh)
uint zox_gpu_mesh_face_corners[24];
byte zox_gpu_mesh_face_corners_made = 0;

// textured: uvs a storage buffer of the tilemap's uvs (TilemapUVs: 24 float2 a block), each vertex gets its uv
static inline void gpu_mesher_set_uvs(GpuMesher* mesher, uint uvs) {
    if (!zox_gpu_mesh_face_corners_made) {
        for (byte i = 0; i < 24; i++) {
            const float3 c = voxel_face_vertices_n[i / 4][i % 4];
            zox_gpu_mesh_face_corners[i] = (c.x > 0.5f) | ((c.y > 0.5f) << 1) | ((c.z > 0.5f) << 2);
        }
        zox_gpu_mesh_face_corners_made = 1;
    }
    mesher->uvs = uvs;
    mesher->textured = uvs != 0;
}

// a grid's bytes uploaded: (2^depth + 2)^3 rounded up to whole uints (depth 0's 27 is 28), grids are made that big
static inline size_t gpu_mesh_grid_bytes(byte depth) {
    const size_t length = octree_size(depth) + 2;
    return (length * length * length + 3) & ~(size_t) 3;
}

// meshes a chunk's grid at depth, the mesh stays on the gpu
//  - voxels: gpu_mesh_grid_bytes(depth) bytes, x then y then z, a voxel of border from the neighbours (smooth_grid's
//      layout, padded)
//  - present: bit OCTREE_INDEX set for each nearby chunk that has voxels (the middle's), a missing one is air
//  - lights (the same layout), lights_present and lighting light it, NULL leaves it unlit (full light)
void gpu_mesh_grid(GpuMesher* mesher, const byte* voxels, const byte* lights, byte depth, uint present, uint lights_present, const GpuMeshLighting* lighting) {
    zox_gpu_storage_write(mesher->command, 0, sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
    zox_gpu_storage_write(mesher->count, 0, sizeof(uint), &zox_gpu_mesh_count_reset);
    const size_t cells = gpu_mesh_grid_bytes(depth);
    // whole new bytes (gl's glBufferData): writing into the buffer an earlier dispatch still reads made vulkan copy its
    //  old bytes to new memory first (~0.18ms a mesh, two 39KB grids), only to overwrite them
    zox_gpu_storage_upload(mesher->voxels, cells, voxels);
    const byte lit = lights && lighting;
    if (lit) {
        zox_gpu_storage_upload(mesher->lights, cells, lights);
    }
    zox_gpu_compute_use(mesher->program);
    zox_gpu_compute_uniform_int(mesher->program, "depth", depth);
    zox_gpu_compute_uniform_uint(mesher->program, "max_faces", mesher->max_faces);
    zox_gpu_compute_uniform_int(mesher->program, "lit", lit);
    zox_gpu_compute_uniform_int(mesher->program, "edge_faces", mesher->edge_faces);
    zox_gpu_compute_uniform_uint(mesher->program, "present", present);
    zox_gpu_compute_uniform_uint(mesher->program, "lights_present", lights_present);
    zox_gpu_compute_uniform_uint_array(mesher->program, "heights", mesher->heights, 32);
    zox_gpu_compute_uniform_uint_array(mesher->program, "drawn", mesher->drawn, 8);
    zox_gpu_compute_uniform_uint_array(mesher->program, "hides", mesher->hides, 8);
    zox_gpu_compute_uniform_int(mesher->program, "textured", mesher->textured);
    if (mesher->textured) {
        zox_gpu_compute_uniform_uint_array(mesher->program, "face_corners", zox_gpu_mesh_face_corners, 24);
    }
    if (lit) {
        zox_gpu_compute_uniform_uint(mesher->program, "darklight", lighting->darklight);
        zox_gpu_compute_uniform_uint(mesher->program, "sunlight", lighting->sunlight);
        zox_gpu_compute_uniform_uint_array(mesher->program, "solid", lighting->solid, 8);
    }
    zox_gpu_storage_bind(0, mesher->voxels);
    zox_gpu_storage_bind(1, mesher->command);
    zox_gpu_storage_bind(8, mesher->count);
    zox_gpu_storage_bind(2, mesher->vertices);
    zox_gpu_storage_bind(3, mesher->indices);
    zox_gpu_storage_bind(4, mesher->lights);
    zox_gpu_storage_bind(5, mesher->vertex_lights);
    zox_gpu_storage_bind(6, mesher->uvs ? mesher->uvs : mesher->vertex_uvs);    // untextured: any buffer, unread
    zox_gpu_storage_bind(7, mesher->vertex_uvs);
    const uint groups = (octree_size(depth) + zox_gpu_mesh_group_size - 1) / zox_gpu_mesh_group_size;
    zox_gpu_compute_dispatch(groups, groups, groups);
    zox_gpu_compute_use(0);
    zox_gpu_fence_dispose(mesher->fence);
    mesher->fence = zox_gpu_fence();
}

// meshes a voxel octree alone (a model, no neighbours) at depth (its own, or lower for a lod), unlit, 1 on success
byte gpu_mesh_VoxelNode(GpuMesher* mesher, const VoxelNode* voxels, byte depth) {
    const int length = octree_size(depth) + 2;
    byte* grid = calloc(gpu_mesh_grid_bytes(depth), 1);
    if (!grid) {
        return 0;
    }
    octree_fill_grid(voxels, sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero, (int3) { 1, 1, 1 },
        grid, length, 0);
    gpu_mesh_grid(mesher, grid, NULL, depth, 1u << OCTREE_INDEX(0, 0, 0), 0, NULL);
    free(grid);
    return 1;
}

// the faces the last mesh wanted, once the gpu's done with it (it never waits): 1 and faces set when it's read
//  - each mesh's count reads once, over max_faces it didn't all fit (gpu_mesher_grow)
byte gpu_mesher_counted(GpuMesher* mesher, uint* faces) {
    if (!mesher->fence || !zox_gpu_fence_done(mesher->fence)) {
        return 0;
    }
    zox_gpu_fence_dispose(mesher->fence);
    mesher->fence = 0;
    *faces = 0;
    zox_gpu_storage_read(mesher->count, 0, sizeof(uint), faces);
    return 1;
}

// room for faces and a quarter more if it has less, 1 when it grew: its mesh is gone, mesh it again
byte gpu_mesher_grow(GpuMesher* mesher, uint faces) {
    if (faces <= mesher->max_faces) {
        return 0;
    }
    mesher->max_faces = faces + faces / 4;
    zox_gpu_storage_upload(mesher->vertices, (size_t) mesher->max_faces * 4 * sizeof(float4), NULL);
    zox_gpu_storage_upload(mesher->vertex_lights, (size_t) mesher->max_faces * 4 * sizeof(uint), NULL);
    zox_gpu_storage_upload(mesher->indices, (size_t) mesher->max_faces * 6 * sizeof(uint), NULL);
    zox_gpu_storage_upload(mesher->vertex_uvs, (size_t) mesher->max_faces * 4 * sizeof(float2), NULL);
    zox_gpu_storage_write(mesher->command, 0, sizeof(zox_gpu_mesh_command_reset), zox_gpu_mesh_command_reset);
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
        mesher->vao = zox_gpu_storage_mesh_vao(mesher->vertices, mesher->indices, 0, mesher->vertex_lights, 1, 0, 2);
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

uint gpu_mesh_textured_program() { return 0; }
uint gpu_mesh_draw_program() { return 0; }
void gpu_mesher_draw(GpuMesher* mesher, uint program, float4x4 camera_matrix, float3 position, float scale, const color_rgb* colors, int colors_count, byte light_mode) { }
byte gpu_mesher_create(GpuMesher* mesher, uint max_faces) {
    (void) max_faces;
    memset(mesher, 0, sizeof(GpuMesher));
    return 0;
}
void gpu_mesher_dispose(GpuMesher* mesher) { memset(mesher, 0, sizeof(GpuMesher)); }
static inline void gpu_mesher_set_heights(GpuMesher* mesher, const byte* heights) { }
static inline void gpu_mesher_set_material(GpuMesher* mesher, const byte* materials, byte material) { }
static inline void gpu_mesher_set_uvs(GpuMesher* mesher, uint uvs) { }
void gpu_mesh_grid(GpuMesher* mesher, const byte* voxels, const byte* lights, byte depth, uint present, uint lights_present, const GpuMeshLighting* lighting) { }
byte gpu_mesh_VoxelNode(GpuMesher* mesher, const VoxelNode* voxels, byte depth) { return 0; }
uint gpu_mesher_read_faces(GpuMesher* mesher) { return 0; }
byte gpu_mesher_counted(GpuMesher* mesher, uint* faces) { return 0; }
byte gpu_mesher_grow(GpuMesher* mesher, uint faces) { return 0; }

#endif
