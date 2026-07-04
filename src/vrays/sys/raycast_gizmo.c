zox_sys2(RaycastGizmoSystem) {
    byte dbg_log = 0;
    byte gizmo_type = 1;
    byte disable_depth = 0;
    float3 hide_position = (float3) { 0, -666, 0 };
    float extrude = 0.001f;
    float shrink = 0.96f;
    float raycaster_quad_thickness = 6;
    color hit_terrain_color = { 255, 255, 255, 66 };
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(RaycastType);
    zox_sys_out(GizmoLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaycastVoxelData, data);
        zox_sys_i(RaycastType, rtype);
        zox_sys_o(GizmoLink, link);
        byte ray_hit = data->result;
        if (ray_hit == rayhit_terrain) {
            float3 position = data->positionf;
            if (gizmo_type == 0) {
                position = float3_add(position, float3_scale(data->normal, quad_depth_buffer));
            } else {
                // For placement rays
                if (rtype->value == 1) {
                    position = float3_add(position, float3_scale(data->normal, data->voxel_scale));
                }
            }
            float4 quad_rotation = quaternion_from_to(float3_up, data->normal);
            float quad_scale = data->voxel_scale * (0.5f - quad_depth_buffer);
            if (!zox_valid(link->value)) {
                if (gizmo_type == 0) {
                    link->value = spawn_quad_lines(world, prefab_quad_lines, hit_terrain_color, position, quad_rotation, raycaster_quad_thickness, quad_scale, 0);
                } else {
                    link->value = spawn_cube_lines_rgba(world, data->positionf, float3_single(0.5f * data->voxel_scale), raycaster_quad_thickness, hit_terrain_color, 0);
                    zox_set(link->value, CubeLineShrink, { shrink });
                    zox_set(link->value, CubeLineExtrude, { extrude });
                    if (disable_depth) {
                        zox_add_tag(link->value, DisableDepthTest);
                    }
                    if (dbg_log) {
                        zox_log("Spawned Gizmo");
                    }
                }
            } else {
                zox_muter(link->value, Position3D, position3);
                if (!float3_equals(position3->value, position)) {
                    position3->value = position;
                    if (dbg_log) {
                        zox_log("Placing Gizmo [%.01fx%.01fx%.01f]", position.x, position.y, position.z);
                    }
                }
                if (gizmo_type == 0) {
                    zox_muter(link->value, Rotation3D, rotation);
                    rotation->value = quad_rotation;
                    zox_muter(link->value, QuadLineSize, scale);
                    scale->value = quad_scale;
                }
            }
            // Debug Line
            if (is_debug_rayhit_point) {
                float3 hit_out = float3_add(data->hit, float3_scale(data->normal, raygizmo_line_length * data->voxel_scale));
                spawn_line3_alpha(world, data->hit, hit_out, raycast_thickness, is_slow_gizmos ? 30 : 0.5f, hit_block_vox_color);
            }
        }
        else if (ray_hit == rayhit_character) {
            // draw a cube above its head instead
            // float3 b = float3_add(data->hit, float3_scale(float3_up, hit_character_line_up));
            /*spawn_line3_thickness_alpha(world, data->hit, b, hit_character_color, raycast_thickness);*/
            // zox_log("hit character alpha %i", hit_character_color.a);
        } else if (ray_hit == rayhit_block_vox) {
            float3 hit_out = float3_add(data->hit, float3_scale(data->normal, hit_block_vox_line_up));
            spawn_line3_alpha(world, data->hit, hit_out, raycast_thickness,  is_slow_gizmos ? 30 : 0.2f, hit_block_vox_color);
        }
        if (ray_hit != rayhit_terrain) {
            if (zox_valid(link->value)) {
                zox_muter(link->value, Position3D, position);
                if (!float3_equals(position->value, hide_position)) {
                    position->value = hide_position;
                    if (dbg_log) {
                        zox_log("Hiding Gizmo");
                    }
                }
            }
        }
    }
} zox_sys_end(RaycastGizmoSystem);
