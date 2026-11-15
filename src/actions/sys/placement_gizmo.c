// the placement gizmo covers a sized block's box (a bed, a door), turned to the holder's facing as placing will
//  (multi_block_place): after raycast_gizmo_system puts it on the one block it would place at
// a sized block's box from its base cell, turned to direction: its centre's offset from the base cell's centre and
//  its half size, in blocks
void placement_box(int3 size, byte direction, float3* centre_offset, float3* extents) {
    const int3 a = block_direction_offset(direction, (int3) { 0, 0, 0 });
    const int3 b = block_direction_offset(direction, (int3) { size.x - 1, size.y - 1, size.z - 1 });
    const float3 low = { int_min(a.x, b.x), int_min(a.y, b.y), int_min(a.z, b.z) };
    const float3 high = { int_max(a.x, b.x), int_max(a.y, b.y), int_max(a.z, b.z) };
    *centre_offset = float3_scale(float3_add(low, high), 0.5f);
    *extents = float3_scale(float3_add(float3_sub(high, low), float3_single(1)), 0.5f);
}

void placement_gizmo_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RaycastVoxelData);
    zox_sys_in(RaycastType);
    zox_sys_in(ActiveAction);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RaycastVoxelData, data);
        zox_sys_i(RaycastType, rtype);
        zox_sys_i(ActiveAction, action);
        const entity gizmo = zox_get_link(world, e, Gizmo);
        if (!zox_valid(gizmo) || !zox_has(gizmo, Bounds3D) || rtype->value != zox_raycast_type_place ||
            data->result != rayhit_terrain) {
            continue;
        }
        const entity item = action->value;
        const entity block = zox_valid(item) ? zox_get_link(world, item, BlockLink) : 0;
        const int3 size = zox_valid(block) && zox_has(block, BlockSize) ? zox_getv(block, BlockSize) : (int3) { 1, 1, 1 };
        const float scale = data->voxel_scale;
        float3 extents = float3_single(0.5f * scale);
        float3 centre = zox_getv(gizmo, Position3D);
        if (size.x > 1 || size.y > 1 || size.z > 1) {
            // its box, turned about the base cell as placing turns it
            float3 offset, half;
            placement_box(size, block_direction_of_placer(world, e), &offset, &half);
            centre = float3_add(centre, float3_scale(offset, scale));
            extents = float3_scale(half, scale);
            zox_setv(gizmo, Position3D, centre);
        }
        if (!float3_equals(zox_getv(gizmo, Bounds3D), extents)) {
            zox_setv(gizmo, Bounds3D, extents);
        }
    }
    zox_sys_on_end();
} zoxd_system(placement_gizmo_system);
