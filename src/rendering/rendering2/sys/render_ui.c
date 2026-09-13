/*
 *  Zox UI Renderer!
 *
 *  NOTE: We are caching the render queue in each system process (flecs processes multiple system calls per table)
 *  NOTE: This is also collected per camera too as they can have their own render data collected
 *  NOTE: The render pass will switch materials for unique ones when needed
 *  NOTE: Uses sorted transparency calls with depth testing for rendering
 *
*/
#define max_renders_count 2048

typedef struct {
    entity e;
    entity material;
    float alpha;
    float brightness;
    float4x4 matrix;
    guint2 mesh;
    guint uvs;
    guint texture;
} ui_render_data;

typedef struct {
    ui_render_data data[max_renders_count];
    int count;
} ui_render_queue;

ui_render_queue zox_ui_render_queue[max_layers2D];
entity shader_matrixui = 0;
entity material_matrixui = 0;

static inline void clear_ui_layers() {
    for (int i = 0; i < max_layers2D; i++) {
        zox_ui_render_queue[i].count = 0;
    }
}

entity spawn_material_matrixui(ecs *world) {
    byte shader_index = get_new_shader_source_index();
    uint material;
    char* vert = get_shader_source(world, "matrixui.vert");
    char* frag = get_shader_source(world, "textured2D.frag");
    shader_verts[shader_index] = vert;
    shader_frags[shader_index] = frag;
    entity shader = spawn_shader(world, shader_index);
    shader_matrixui = shader;
    if (!shader) {
        zox_log_error("=> [spawn_shader_textured2D] Failed:\n%s", vert);
        return 0;
    }
    zox_set_unique_name(shader, "shader_matrixui");
    entity e = spawn_material(world, shader, &material);
    material_matrixui = e;
    if (!e) {
        zox_loge("Material [matrixui] Failed");
        return 0;
    }
    zox_set(e, ShaderLink, { shader });
    attributes_matrixui base_attributes = create_attributes_matrixui(material);
    zox_set_data(e, attributes_matrixui, base_attributes);
    return e;
}

void render_uis(ecs* world, ui_render_queue* uis) {
    byte dbg_log = 0;
    float depth_per_layer = 3 * zox_depth_per_layer;
    float depth_begin = depth_per_layer;
    const attributes_matrixui* attributes = NULL;
    entity material = 0;
    if (dbg_log) {
        zox_log("Rendering UI [%i] Layers", max_layers2D);
    }
    for (int layer = 0; layer < max_layers2D; layer++) {
        ui_render_queue* queue = &uis[layer];
        if (dbg_log && queue->count) {
            zox_log(" - Layer [%i] x[%i]", layer, queue->count);
        }
        for (int j = 0; j < queue->count; j++) {
            ui_render_data data = queue->data[j];
            entity new_material = data.material;
            // NOTE: Sets the material here
            if (material != new_material) {
#ifdef zox_safety_checks
                if (!zox_valid(new_material)) {
                    zox_loge("Invalid UI Material TransformUI");
                    continue;
                }
                if (!zox_has(new_material, attributes_matrixui)) {
                    zox_loge("[%s] has Invalid UI Material [%s] no [attributes_matrixui]", zox_getn(data.e), zox_getn(new_material));
                    continue;
                }
#endif
                material = new_material;
                attributes = zox_get(material, attributes_matrixui);
                guint material_id = zox_getv(material, MaterialGPULink);
                zox_gpu_material(material_id);
                zox_gpu_float4x4(attributes->camera_matrix, render_camera_matrix);
                if (zox_has(material, MaterialBlur) && zox_has(material, CameraBlur)) {
                    guint property_id = zox_getv(material, MaterialBlur);
                    float property_value = zox_getv(material, CameraBlur);
                    zox_gpu_float(property_id, property_value);
                    if (dbg_log >= 2) {
                        zox_log("- Set Material Blur [%i] to [%f]", property_id, property_value);
                    }
                }
                if (zox_has(material, MaterialVignette) && zox_has(material, CameraVignette)) {
                    guint property_id = zox_getv(material, MaterialVignette);
                    float property_value = zox_getv(material, CameraVignette);
                    zox_gpu_float(property_id, property_value);
                    if (dbg_log >= 2) {
                        zox_log("- Set Material Vignette [%i] to [%f]", property_id, property_value);
                    }
                }
                zox_gpu_enable_blend();
                zox_gpu_enable_depth_test();
                zox_gpu_set_depth_mask(0);
            }
            // NOTE: Material might of not been set
            if (!material) {
                continue;
            }
            // Set layer depth
            float4x4 matrix2 = data.matrix;
            matrix2.w.z = depth_begin + layer * depth_per_layer;
            // per mesh data
            zox_gpu_bind_buffer_element(data.mesh.x);
            zox_gpu_bind_texture(data.texture);
            zox_gpu_int(attributes->texture, 0);
            zox_gpu_bind_buffer_array(data.mesh.y);
            zox_gpu_enable_attribute_float2(attributes->vertex_position);
            zox_gpu_bind_buffer_array(data.uvs);
            zox_gpu_enable_attribute_float2(attributes->vertex_uv);
            zox_gpu_float4x4(attributes->matrix, matrix2);
            zox_gpu_float(attributes->brightness, data.brightness);
            zox_gpu_float(attributes->alpha, data.alpha);
            zox_gpu_render(6);
            if (dbg_log) {
                zox_log("   -- [%s] at L[%i] Depth [%f]", zox_getn(data.e), layer, matrix2.w.z);
                if (dbg_log >= 2) {
                    zox_log("- Brightness [%f] Alpha [%f]", data.brightness, data.alpha);
                    zox_log("- GPU: Mesh [%ix%i] UVs [%i] Texture [%i]", data.mesh.x, data.mesh.y, data.uvs, data.texture);
                }
            }
        }
    }
    if (material) {
        zox_gpu_disable_attribute(attributes->vertex_uv);
        zox_gpu_disable_attribute(attributes->vertex_position);
        zox_gpu_reset_mesh();
        zox_gpu_reset_texture();
        zox_disable_material();
        zox_gpu_disable_blend();
        zox_gpu_disable_depth_test();
        zox_gpu_set_depth_mask(1);
    }
}

// NOTE: Collects our Render Queue for UIs
zox_sys2(ElementRenderSystem) {
    byte dbg_log = 0;
    entity base_material = material_matrixui;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(TransformMatrix);
    zox_sys_in(Layer);
    zox_sys_in(Brightness);
    zox_sys_in(Alpha);
    zox_sys_in(MeshGPULink);
    zox_sys_in(UvsGPULink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Layer, layer);
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Alpha, alpha);
        zox_sys_i(Brightness, brightness);
        zox_sys_i(TransformMatrix, matrix);
        zox_sys_i(MeshGPULink, mesh);
        zox_sys_i(UvsGPULink, uvs);
        zox_sys_i(TextureGPULink, texture);
        if (disabled->value || !alpha->value) {
            continue;
        }
        // Filter our camera
        entity render_camera = zox_get_mesh2_camera(world, e);
        if (render_camera != renderer_camera) {
            if (dbg_log) {
                zox_logw("Not Rendering [%s] at L[%i], Cameras [%s]:[%s]", zox_getn(e), layer, zox_getn(render_camera), zox_getn(renderer_camera));
            }
            continue;
        }
#ifdef zox_safety_checks
        if (!mesh->value.x || !mesh->value.y || !uvs->value || !texture->value) {
            if (dbg_log) {
                zox_logw("Mesh Invalid [%s] at L[%i]", zox_get_name(e), layer);
            }
            continue;
        }
#endif
        ui_render_queue* queue = &zox_ui_render_queue[layer->value];
        if (queue->count >= max_renders_count) {
            zox_loge("UI Render Layer Over Max [%i]", layer->value);
            continue;
        }
        entity material = zox_get_link(world, e, MaterialLink);
        if (!material) {
            material = base_material;
        }
        // entity material = zox_has(e, MaterialLink) ? zox_getv(e, MaterialLink) : base_material;
        if (!zox_valid(material)) {
            continue;
        }
        ui_render_data data = {
            .e = e,
            .material = material,
            .alpha = alpha->value,
            .brightness = brightness->value,
            .matrix = matrix->value,
            .mesh = mesh->value,
            .uvs = uvs->value,
            .texture = texture->value,
        };
        queue->data[queue->count] = data;
        queue->count++;
    }
} zox_sys_end(ElementRenderSystem);

