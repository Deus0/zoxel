extern entity local_crosshair;
extern void crosshair_set_type(ecs*, entity, byte);
byte is_debug_mid_voxel = 0;
byte is_slow_gizmos = 0;
float raygizmo_line_length = 0.13f;   // 0.2f

// using DDA for raycasting
byte create_raycast_gizmo(
    ecs *world,
    const RaycastVoxelData *data
) {
    byte ray_hit = data->result;

    if (ray_hit == rayhit_terrain) {
        // zox_log("data->voxel_scale: %f", data->voxel_scale);

        // add line too
        float3 b = float3_add(
            data->hit, // hit positionf
            float3_scale(data->normal, raygizmo_line_length * data->voxel_scale));

        spawn_line3D_colored_alpha(
            world,
            data->hit, // hit positionf
            b,
            raycast_thickness,
            is_slow_gizmos ? 30 : 0.5f,
            hit_block_vox_color
        );

        // pointf is in middle of block
        float3 center_quad = float3_add(
            data->positionf,
            float3_scale(data->normal, data->voxel_scale * (0.501f)));

        if (is_debug_mid_voxel) {
            spawn_line3D_colored_alpha(
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

        other_axis = float3_scale(other_axis,
            data->voxel_scale * 0.5f - data->voxel_scale * (0.125f / raycast_thickness));
        if (data->normal.z != 0) {
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
                float3_add(center_quad, (float3) { -other_axis.x, other_axis.y, other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { -other_axis.x, other_axis.y, other_axis.z }),
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { other_axis.x, -other_axis.y, -other_axis.z }),
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
                float3_add(center_quad, (float3) { other_axis.x, -other_axis.y, -other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);

        } else {
            // handles x and y
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, other_axis.z }),
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, -other_axis.z }),
                float3_add(center_quad, (float3) { -other_axis.x, -other_axis.y, -other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
            render_line3D_thickness_alpha(world,
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, other_axis.z }),
                float3_add(center_quad, (float3) { other_axis.x, other_axis.y, -other_axis.z }),
                hit_terrain_color, raycaster_quad_thickness);
        }
        // zox_log(" > h [%fx%fx%f]\n", data->hit.x, data->hit.y, data->hit.z)
        // zox_log(" > r [%fx%fx%f]\n", data->positionf.x, data->positionf.y, data->positionf.z)
    }

    else if (ray_hit == rayhit_character) {
        // draw a cube above its head instead
        float3 b = float3_add(data->hit, float3_scale(float3_up, 0.06f));
        render_line3D_thickness_alpha(world, data->hit, b, hit_character_color, raycast_thickness);
    }

    else if (ray_hit == rayhit_block_vox) {
        float3 b = float3_add(data->hit, float3_scale(data->normal, 0.06f));
        spawn_line3D_colored_alpha(
            world,
            data->hit,
            b,
            0.5f,
            raycast_thickness,
            hit_block_vox_color
        );
    }

    return ray_hit;
}

void RaycastGizmoSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RaycastVoxelData, data);
        crosshair_set_type(world, local_crosshair, data->result);
        create_raycast_gizmo(world, data);
    }
} zoxd_system(RaycastGizmoSystem)
