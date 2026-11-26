// Draws a mesh's instances (vox_instance_render_system): grass, minivoxes, characters
//  - each mesh one command: its instances' model matrices, drawn instanced, a uniform buffer's worth at a time
//    (zox_get_safe_ubo_size: past it the buffer overflowed, gl errors and missing instances, tst vox_instances_stress)
//  - instances outside the camera's view aren't sent (a sphere against its planes, CameraPlanes), each mesh's radius
//    its farthest vertex from its centre, once a frame
//  - the commands and their matrix lists are kept between frames (rendering is on the main thread), not made and
//    freed every frame
#define zox_instance_meshes_max 64
InstanceRenderCommand zox_instance_commands[zox_instance_meshes_max];
float zox_instance_radius[zox_instance_meshes_max];
int zox_instance_commands_count = 0;
int zox_instance_ubo_max = 0;

// a mesh's bounding radius: its farthest vertex from its centre (instances are placed by it), 1 if it has none
static inline float instance_mesh_radius(ecs* world, entity mesh) {
    const MeshVertices* vertices = zox_has(mesh, MeshVertices) ? zox_get(mesh, MeshVertices) : NULL;
    if (!vertices || !vertices->length) {
        return 1;
    }
    float most = 0;
    for (int i = 0; i < vertices->length; i++) {
        const float3 v = vertices->value[i];
        most = float_max(most, v.x * v.x + v.y * v.y + v.z * v.z);
    }
    return sqrtf(most) + 0.1f;  // and the sway (grass tips move zox_grass_sway_strength)
}

// a model mesh's see through sibling at its lod (TransparentMesh, set_vox_lods_materials), 0 if its model has none
static inline entity instance_see_through_of(ecs* world, entity mesh) {
    const entity model = zox_get_parent(world, mesh);
    if (!zox_valid(model) || !zox_has(mesh, RenderDepth)) {
        return 0;
    }
    const byte depth = zox_getv(mesh, RenderDepth);
    entity found = 0;
    iter it = zox_children(world, model);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count && !found; i++) {
            const entity child = it.entities[i];
            if (child != mesh && zox_has(child, TransparentMesh) && zox_has(child, RenderDepth) &&
                zox_getv(child, RenderDepth) == depth)
            {
                found = child;
            }
        }
    }
    return found;
}

// the command for a mesh this frame, a new one if it has none (-1 when full)
static inline int instance_command_of(ecs* world, entity mesh, byte sway) {
    for (int i = 0; i < zox_instance_commands_count; i++) {
        if (zox_instance_commands[i].mesh == mesh) {
            return i;
        }
    }
    if (zox_instance_commands_count == zox_instance_meshes_max) {
        return -1;
    }
    const int i = zox_instance_commands_count++;
    InstanceRenderCommand* command = &zox_instance_commands[i];
    if (!command->transforms) {
        command->transforms = create_float4x4_array_d(256);
    }
    command->transforms->size = 0;
    command->mesh = mesh;
    command->sway = sway;
    command->see_through = instance_see_through_of(world, mesh);
    zox_instance_radius[i] = instance_mesh_radius(world, mesh);
    return i;
}

// Draws one mesh for all its instances, material must already be bound
static inline void render_vox_instance_command(
    ecs* world,
    InstanceRenderCommand command,
    gint vertex_position,
    gint vertex_color,
    guint ubo)
{
    entity mesh = command.mesh;
    if (!command.transforms || !command.transforms->size || !zox_valid(mesh)) {
        return;
    }
    if (!zox_has(mesh, MeshIndicies)) {
        zox_loge("Invalid Instance Mesh [%s]: No MeshIndicies", zox_get_name(mesh));
        return;
    }
    // Ignore if no indicies; a gpu mesh (chunks3/sys/vox_gpu.c) has its count, its cpu arrays empty
    const byte gpu = zox_has(mesh, GpuMesh);
    const int indices = mesh_index_count(world, mesh);
    if (!indices) {
        return;
    }
    if (!zox_has(mesh, MeshGPULink)) {
        zox_loge("Invalid Instance Mesh [%s]: No MeshGPULink", zox_get_name(mesh));
        return;
    }
    const MeshGPULink* gpumesh = zox_get(mesh, MeshGPULink);
    if (!gpumesh->value.x || !gpumesh->value.y) {
        zox_loge("Invalid Instance Mesh [%s]: GPU Mesh is 0.", zox_get_name(mesh));
        return;
    }
    const ColorsGPULink* gpucolors = zox_get(mesh, ColorsGPULink);
    if (!gpucolors->value) {
        zox_loge("Invalid Instance Mesh [%s]: GPU Color is 0.", zox_get_name(mesh));
        return;
    }
    zox_gpu_bind_buffer_element(gpumesh->value.x);
    if (gpu) {
        // the compute shader's: vec4 vertices, rgba8 colours
        zox_gpu_array_buffer_float3_stride(vertex_position, gpumesh->value.y, sizeof(float4));
        zox_gpu_array_buffer_byte4(vertex_color, gpucolors->value);
    } else {
        opengl_enable_vertex_buffer(vertex_position, gpumesh->value.y);
        opengl_enable_color_buffer(vertex_color, gpucolors->value);
    }
    // a uniform buffer's worth a draw
    const int count = command.transforms->size;
    const int batch = zox_instance_ubo_max > 0 ? zox_instance_ubo_max : count;
    for (int start = 0; start < count; start += batch) {
        const int render_count = count - start < batch ? count - start : batch;
        zox_gpu_ubo_stream_matricies(ubo, render_count, batch, command.transforms->data + start);
        zox_gpu_render_triangles_instanced(indices, render_count);
    }
    zox_drawn_vox_instances += count;
    zox_gpu_ubo_reset();
    zox_gpu_disable_attribute(vertex_color);
    zox_gpu_disable_attribute(vertex_position);
    zox_gpu_reset_mesh();
}

// Renders Characters and Mimuvoxes
void vox_instance_render_system(iter* it) {
    zox_sys_on_begin();
    const double render_begin = get_time_ms();
#ifndef zox_disable_ubos
    if (!material_vox_instance) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    zox_sys_in(Brightness);
    camera_filtering_begin();
    guint material_link = zox_getv(material_vox_instance, MaterialGPULink);
    if (!material_link) {
        return;
    }
    const UboGPULink* ubo = zox_get(material_vox_instance, UboGPULink);
    if (!ubo->value) {
        return;
    }
    if (!zox_instance_ubo_max) {
        zox_instance_ubo_max = zox_get_safe_ubo_size();
    }
    const MaterialVoxInstance* material_attributes = zox_get(material_vox_instance, MaterialVoxInstance);
    // the camera's view to cull by, none for one without planes (XR's eyes: renderer_camera is 0)
    const plane* planes = zox_valid(renderer_camera) && zox_has(renderer_camera, CameraPlanes) ?
        zox_get(renderer_camera, CameraPlanes)->value : NULL;
    zox_instance_commands_count = 0;
    entity last_mesh = 0;
    int index = -1;
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(Brightness, brightness);
        if (disabled->value) {
            continue;
        }
        const entity instance = zox_get_link(world, it->entities[i], InstanceLink);
        if (!zox_valid(instance)) {
            continue;
        }
        camera_filtering_check();
        // a table's instances are mostly the same mesh in a row
        if (instance != last_mesh) {
            // NOTE: sway meshes (grass) use the sway material, if animation is on
            const byte sway = zox_grass_animation && material_vox_instance_sway && zox_has(instance, SwayMesh);
            index = instance_command_of(world, instance, sway);
            last_mesh = instance;
        }
        if (index < 0) {
            continue;
        }
        // outside the view: not sent (its radius scaled as the instance is, a column of its matrix)
        if (planes) {
            const float3 center = { matrix->value.w.x, matrix->value.w.y, matrix->value.w.z };
            const float scale = float3_length((float3) { matrix->value.x.x, matrix->value.x.y, matrix->value.x.z });
            if (!is_sphere_in_frustum(planes, center, zox_instance_radius[index] * scale)) {
                continue;
            }
        }
        // model matrices, the shaders apply the camera (sway moves verts in world space first)
        //  - the unused bottom row x carries the brightness, i.e. the light at a minivox
        float4x4 instance_matrix = matrix->value;
        instance_matrix.x.w = brightness->value;
        float4x4_array_d_add(zox_instance_commands[index].transforms, instance_matrix);
    }
    // Static vox instances
    zox_gpu_material(material_link);
    zox_gpu_float4(material_attributes->fog_data, get_fog_value());
    zox_gpu_float(material_attributes->brightness, 1);
    zox_gpu_float(material_attributes->alpha, 1);
    zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
    outline_mask_begin(material_attributes->outline);
    byte has_sway = 0;
    byte has_see_through = 0;
    for (int i = 0; i < zox_instance_commands_count; i++) {
        InstanceRenderCommand command = zox_instance_commands[i];
        has_see_through |= command.see_through != 0;
        if (command.sway) {
            has_sway = 1;
            continue;
        }
        render_vox_instance_command(
            world,
            command,
            material_attributes->vertex_position,
            material_attributes->vertex_color,
            ubo->value);
    }
    outline_mask_end();     // grass isn't outlined
    // Swaying vox instances (grass)
    guint sway_material_link = has_sway ?
        zox_getv(material_vox_instance_sway, MaterialGPULink) :
        0;
    if (sway_material_link) {
        const MaterialVoxInstanceSway* sway_attributes = zox_get(material_vox_instance_sway, MaterialVoxInstanceSway);
        // wrap time to keep float precision in the shader
        float sway_time = (float) fmod(zox_current_time, 3600.0);
        zox_gpu_material(sway_material_link);
        zox_gpu_float4(sway_attributes->fog_data, get_fog_value());
        zox_gpu_float(sway_attributes->brightness, 1);
        zox_gpu_float(sway_attributes->alpha, 1);
        zox_gpu_float4x4(sway_attributes->camera_matrix, render_camera_matrix);
        for (int i = 0; i < zox_instance_commands_count; i++) {
            InstanceRenderCommand command = zox_instance_commands[i];
            if (!command.sway) {
                continue;
            }
            float2 bounds = zox_has(command.mesh, SwayBounds) ?
                zox_getv(command.mesh, SwayBounds) :
                (float2) { -0.5f, 1.0f };
            zox_gpu_float4(sway_attributes->sway_data, (float4) {
                sway_time,
                zox_grass_sway_strength,
                bounds.x,
                bounds.y
            });
            render_vox_instance_command(
                world,
                command,
                sway_attributes->vertex_position,
                sway_attributes->vertex_color,
                ubo->value);
        }
    }
    // see through meshes (a slime's shell): after every solid one, the same instances, blended; tested against depth
    //  but not writing it, so a shell's far faces aren't hidden by its near ones drawn first (thin lines through it)
    if (has_see_through) {
        zox_gpu_material(material_link);
        zox_gpu_float4(material_attributes->fog_data, get_fog_value());
        zox_gpu_float(material_attributes->brightness, 1);
        zox_gpu_float4x4(material_attributes->camera_matrix, render_camera_matrix);
        zox_gpu_enable_blend();
        zox_gpu_set_depth_mask(0);
        outline_mask_begin(material_attributes->outline);
        for (int i = 0; i < zox_instance_commands_count; i++) {
            InstanceRenderCommand command = zox_instance_commands[i];
            if (!command.see_through) {
                continue;
            }
            command.mesh = command.see_through;
            zox_gpu_float(material_attributes->alpha, instance_see_through_alpha(world, command.mesh));
            render_vox_instance_command(world, command, material_attributes->vertex_position,
                material_attributes->vertex_color, ubo->value);
        }
        outline_mask_end();
        zox_gpu_set_depth_mask(1);
        zox_gpu_disable_blend();
    }
    zox_disable_material();
    catch_basic3D_errors("vox_instance_render_system");
    zox_vox_instance_render_ms += get_time_ms() - render_begin;
#endif
    zox_sys_on_end();
} zoxd_system(vox_instance_render_system);
