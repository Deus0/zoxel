extern entity local_crosshair;
extern void crosshair_set_type(ecs*, entity, byte);
const float quad_depth_buffer = 0.02f;

// TODO: Use only Local Voxel Position With Chunk to get PositionF of Block - Cleaner code

zox_sys2(RaycastGizmoSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_out(GizmoLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaycastVoxelData, data);
        zox_sys_o(GizmoLink, link);

        crosshair_set_type(world, local_crosshair, data->result);

        byte ray_hit = data->result;
        if (ray_hit == rayhit_terrain) {

            float3 quad_position = float3_add(data->positionf, float3_scale(data->normal, quad_depth_buffer));
            float4 quad_rotation = quaternion_from_to(float3_up, data->normal);
            float quad_scale = data->voxel_scale * (0.5f - quad_depth_buffer);

            if (!zox_valid(link->value)) {
                link->value = spawn_quad_lines(
                    world,
                    prefab_quad_lines,
                    hit_terrain_color,
                    quad_position,
                    quad_rotation,
                    raycaster_quad_thickness,
                    quad_scale,
                    0
                );
            } else {
                zox_muter(link->value, Position3D, position);
                zox_muter(link->value, Rotation3D, rotation);
                zox_muter(link->value, QuadLineSize, scale);
                position->value = quad_position;
                rotation->value = quad_rotation;
                scale->value = quad_scale;
            }

            // Debug Line
            if (is_debug_rayhit_point) {
                float3 hit_out = float3_add(
                    data->hit, // hit positionf
                    float3_scale(data->normal, raygizmo_line_length * data->voxel_scale));

                spawn_line3_alpha(
                    world,
                    data->hit, // hit positionf
                    hit_out,
                    raycast_thickness,
                    is_slow_gizmos ? 30 : 0.5f,
                    hit_block_vox_color
                );
            }
        }

        else if (ray_hit == rayhit_character) {
            // draw a cube above its head instead
            float3 b = float3_add(data->hit, float3_scale(float3_up, hit_character_line_up));
            spawn_line3_thickness_alpha(
                world,
                data->hit,
                b,
                hit_character_color,
                raycast_thickness
            );
            // zox_log("hit character alpha %i", hit_character_color.a);
        }

        else if (ray_hit == rayhit_block_vox) {
            float3 hit_out = float3_add(data->hit, float3_scale(data->normal, hit_block_vox_line_up));
            spawn_line3_alpha(
                world,
                data->hit, // hit positionf
                hit_out,
                raycast_thickness,
                is_slow_gizmos ? 30 : 0.2f,
                hit_block_vox_color
            );
        }

        if (ray_hit != rayhit_terrain) {
            if (zox_valid(link->value)) {
                // zox_log("hiding gizmo");
                float3 hide_position = (float3) { 0, -666, 0 };
                zox_muter(link->value, Position3D, position);
                position->value = hide_position;
            }
        }
    }
} zox_sys_end(RaycastGizmoSystem);


/*if (is_debug_mid_voxel) {
    spawn_line3_alpha(
        world,
        data->positionf,
        data->hit,
        raycast_thickness,
        5,
        color_red
    );
}

float3 other_axis = float3_zero;
if (data->normal.y != 0) {
    other_axis.x = 1;
    other_axis.z = 1;
} else if (data->normal.x != 0) {
    other_axis.y = 1;
    other_axis.z = 1;
} else if (data->normal.z != 0) {
    other_axis.x = 1;
    other_axis.y = 1;
}

other_axis = float3_scale(other_axis, axis_scale);
if (data->normal.z != 0) {
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
        float3_add(center_quad, (float3) { -other_axis.x, other_axis.y, other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { -other_axis.x, other_axis.y, other_axis.z }),
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { other_axis.x, -other_axis.y, -other_axis.z }),
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
        float3_add(center_quad, (float3) { other_axis.x, -other_axis.y, -other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);

} else {
    // handles x and y
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, other_axis.z }),
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, -other_axis.z }),
        float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
    spawn_line3_thickness_alpha(world,
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
        float3_add(center_quad, (float3) { other_axis.x, other_axis.y, -other_axis.z }),
        hit_terrain_color, raycaster_quad_thickness);
}
// zox_log(" > h [%fx%fx%f]\n", data->hit.x, data->hit.y, data->hit.z)
// zox_log(" > r [%fx%fx%f]\n", data->positionf.x, data->positionf.y, data->positionf.z)*/
