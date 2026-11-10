// todo:
//   create vbo for instance stuff like particles
//   create instanced mesh prefab and spawn for grass - just has to have a tag, transform data, and point to a mesh to render
// atm renders about 2k per frame
// when registering a new instance entity:
// create a renderer data:
//      - material
//      - ubo link
//      - transform list
// extern string_hashmap *files_hashmap_voxes;

// Draws one mesh for all its instances, material must already be bound
static inline void render_vox_instance_command(
    ecs* world,
    InstanceRenderCommand command,
    gint vertex_position,
    gint vertex_color,
    guint ubo)
{
    entity mesh = command.mesh;
    if (!command.transforms || !zox_valid(mesh)) {
        return;
    }
    if (!zox_has(mesh, MeshIndicies)) {
        zox_loge("Invalid Instance Mesh [%s]: No MeshIndicies", zox_get_name(mesh));
        return;
    }
    // Ignore if no indicies
    zox_geter(mesh, MeshIndicies, meshIndicies);
    if (!meshIndicies->length) {
        return;
    }
    if (!zox_has(mesh, MeshGPULink)) {
        zox_loge("Invalid Instance Mesh [%s]: No MeshGPULink", zox_get_name(mesh));
        return;
    }
    zox_geter(mesh, MeshGPULink, gpumesh);
    if (!gpumesh->value.x || !gpumesh->value.y) {
        zox_loge("Invalid Instance Mesh [%s]: GPU Mesh is 0.", zox_get_name(mesh));
        return;
    }
    zox_geter(mesh, ColorsGPULink, gpucolors);
    if (!gpucolors->value) {
        zox_loge("Invalid Instance Mesh [%s]: GPU Color is 0.", zox_get_name(mesh));
        return;
    }
    zox_gpu_bind_buffer_element(gpumesh->value.x);
    opengl_enable_vertex_buffer(vertex_position, gpumesh->value.y);
    opengl_enable_color_buffer(vertex_color, gpucolors->value);
    int render_count = command.transforms->size;
    zox_gpu_ubo_set_matricies(
        ubo,
        render_count,
        command.transforms->data);
    zox_gpu_render_triangles_instanced(meshIndicies->length, render_count);
    zox_gpu_ubo_reset();
    zox_gpu_disable_attribute(vertex_color);
    zox_gpu_disable_attribute(vertex_position);
    zox_gpu_reset_mesh();
}

// Renders Characters and Mimuvoxes
void vox_instance_render_system(iter* it) {
    zox_sys_on_begin();
#ifndef zox_disable_ubos
    if (!material_vox_instance) {
        return;
    }
    uint max_meshes = 16;
    uint max_transforms = 2048;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TransformMatrix);
    zox_sys_in(RenderDisabled);
    camera_filtering_begin();
    guint material_link = zox_getv(material_vox_instance, MaterialGPULink);
    if (!material_link) {
        return;
    }
    zox_geter(material_vox_instance, UboGPULink, ubo);
    if (!ubo->value) {
        // zox_log(" ! error with material_vox_instance uboGPULink.\n")
        return;
    }
    zox_geter(material_vox_instance, MaterialVoxInstance, material_attributes);
    InstanceRenderCommand_array_d* commands = create_InstanceRenderCommand_array_d(max_meshes);
    if (!commands->data) {
        // errored out
        zox_loge("(vox_instance_render_system) commands->data malloc failed");
        return;
    }
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(TransformMatrix, matrix);
        if (disabled->value) {
            continue;
        }
        const entity instance = zox_get_link(world, it->entities[i], InstanceLink);
        if (!zox_valid(instance)) {
            continue;
        }
        camera_filtering_check();
        int index = 0;
        if (!has_mesh(commands, instance, &index)) {
            // NOTE: sway meshes (grass) use the sway material, if animation is on
            byte sway = zox_grass_animation &&
                material_vox_instance_sway &&
                zox_has(instance, SwayMesh);
            InstanceRenderCommand command = {
                .mesh = instance,
                .sway = sway,
                .transforms = create_float4x4_array_d(max_transforms)
            };
            if (!command.transforms) {
                // errored out
                zox_loge("(vox_instance_render_system) command.transforms malloc failed");
                break;
            }
            InstanceRenderCommand_array_d_add(commands, command);
            index = commands->size - 1;
        }
        InstanceRenderCommand command = commands->data[index];
        if (!command.transforms) {
            zox_loge("(vox_instance_render_system) command.transforms malloc failed");
            break;
        }
        // sway shader applies camera after moving verts in world space
        float4x4 instance_matrix = command.sway ?
            matrix->value :
            float4x4_multiply(
                matrix->value,
                render_camera_matrix);
        float4x4_array_d_add(command.transforms, instance_matrix);
        commands->data[index] = command;
    }
    // Static vox instances
    zox_gpu_material(material_link);
    zox_gpu_float4(material_attributes->fog_data, get_fog_value());
    zox_gpu_float(material_attributes->brightness, 1);
    byte has_sway = 0;
    for (size_t i = 0; i < commands->size; i++) {
        InstanceRenderCommand command = commands->data[i];
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
    // Swaying vox instances (grass)
    guint sway_material_link = has_sway ?
        zox_getv(material_vox_instance_sway, MaterialGPULink) :
        0;
    if (sway_material_link) {
        zox_geter(material_vox_instance_sway, MaterialVoxInstanceSway, sway_attributes);
        // wrap time to keep float precision in the shader
        float sway_time = (float) fmod(zox_current_time, 3600.0);
        zox_gpu_material(sway_material_link);
        zox_gpu_float4(sway_attributes->fog_data, get_fog_value());
        zox_gpu_float(sway_attributes->brightness, 1);
        zox_gpu_float4x4(sway_attributes->camera_matrix, render_camera_matrix);
        for (size_t i = 0; i < commands->size; i++) {
            InstanceRenderCommand command = commands->data[i];
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
    // cleanup
    for (size_t i = 0; i < commands->size; i++) {
        InstanceRenderCommand command = commands->data[i];
        dispose_float4x4_array_d(command.transforms);
    }
    dispose_InstanceRenderCommand_array_d(commands);
    zox_disable_material();
    catch_basic3D_errors("vox_instance_render_system");
#endif
    zox_sys_on_end();
} zoxd_system(vox_instance_render_system);
