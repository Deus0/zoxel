zox_sys2(RaycastGizmoSystem) {
    byte dbg_log = 0;
    byte gizmo_type = 1;
    byte disable_depth = 0; // 1;
    float3 hide_position = (float3) { 0, -999, 0 };
    color hit_terrain_color = { 0, 0, 0, 255 };
    float extrude = 0.01f;  // 0.0001f;
    float shrink = 1;       // 0.99f;
    float raycaster_quad_thickness = 4;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(RaycastType);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RaycastVoxelData, data);
        zox_sys_i(RaycastType, rtype);
        byte ray_hit = data->result;
        entity gizmo = zox_get_link(world, e, Gizmo);
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
            byte sides = get_raycast_sides(world, data);
            if (rtype->value == 1) {
                sides = 255; // all sides
            }
            float4 quad_rotation = quaternion_from_to(float3_up, data->normal);
            float quad_scale = data->voxel_scale * (0.5f - quad_depth_buffer);
            if (!zox_valid(gizmo)) {
                if (gizmo_type == 0) {
                    gizmo = spawn_quad_lines(
                        world,
                        prefab_quad_lines,
                        hit_terrain_color,
                        position,
                        quad_rotation, raycaster_quad_thickness,
                        quad_scale,
                        0);
                } else {
                    gizmo = spawn_cube_lines_rgba(
                        world,
                        data->positionf,
                        float3_single(0.5f * data->voxel_scale),
                        raycaster_quad_thickness,
                        hit_terrain_color,
                        0);
                    zox_setv(gizmo, CubeLineShrink, shrink);
                    zox_setv(gizmo, CubeLineExtrude, extrude);
                    zox_setv(gizmo, CubeLineSides, sides);
                    if (disable_depth) {
                        zox_add(gizmo, DisableDepthTest);
                    }
                    if (dbg_log) {
                        zox_log("Spawned Gizmo");
                    }
                }
                zox_link(world, e, Gizmo, gizmo);
            } else {
                zox_muter(gizmo, Position3D, position3);
                if (!float3_equals(position3->value, position)) {
                    position3->value = position;
                    if (dbg_log) {
                        zox_log("Placing Gizmo [%.01fx%.01fx%.01f]",
                            position.x,
                            position.y,
                            position.z);
                    }
                }
                if (gizmo_type == 0) {
                    zox_muter(gizmo, Rotation3D, rotation);
                    rotation->value = quad_rotation;
                    zox_muter(gizmo, QuadLineSize, scale);
                    scale->value = quad_scale;
                } else {
                    // CubeLineSides
                    zox_set(gizmo, CubeLineSides, { sides });
                }
            }
            // Debug Line
            if (is_debug_rayhit_point) {
                float3 hit_out = float3_add(data->hit, float3_scale(data->normal, raygizmo_line_length * data->voxel_scale));
                spawn_line3_alpha(
                    world,
                    data->hit,
                    hit_out,
                    raycast_thickness,
                    is_slow_gizmos ?
                        30 :
                        0.5f,
                    hit_block_vox_color);
            }
        }
        else if (ray_hit == rayhit_character) {
            // draw a cube above its head instead
            // float3 b = float3_add(data->hit, float3_scale(float3_up, hit_character_line_up));
            /*spawn_line3_thickness_alpha(world, data->hit, b, hit_character_color, raycast_thickness);*/
            // zox_log("hit character alpha %i", hit_character_color.a);
        } else if (ray_hit == rayhit_block_vox) {
            float3 hit_out = float3_add(data->hit, float3_scale(data->normal, hit_block_vox_line_up));
            spawn_line3_alpha(
                world,
                data->hit,
                hit_out,
                raycast_thickness,
                is_slow_gizmos ?
                    30 :
                    0.2f,
                hit_block_vox_color);
        }
        if (ray_hit != rayhit_terrain) {
            if (zox_valid(gizmo)) {
                zox_muter(gizmo, Position3D, position);
                if (!float3_equals(position->value, hide_position)) {
                    position->value = hide_position;
                    if (dbg_log) {
                        zox_log("Hiding Gizmo Hit Type [%i]", ray_hit);
                    }
                }
            }
        }
    }
} zox_sys_end(RaycastGizmoSystem);
