// tst: placement_box, the placement gizmo's box for a sized block (actions/sys/placement_gizmo.c)
//  - a bed (2 x 1 x 1) turned each way: its box over its two cells (the base and its turned filler), 2 blocks long
//    along x or z, 1 across; a door (1 x 2 x 1) any way: 1 x 2 x 1, half a block up from its base cell
//  - the box matches multi_block_place's cells (block_direction_offset)
//  - run: bash bsh/tst.sh placement_box
byte zox_tst_placement_box_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_placement_box_check(ecs* world) {
    (void) world;
    for (byte direction = 0; direction < 4; direction++) {
        float3 offset, extents;
        placement_box((int3) { 2, 1, 1 }, direction, &offset, &extents);
        // its filler's cell, as placing turns it: the box's centre halfway to it, long that way
        const int3 filler = block_direction_offset(direction, (int3) { 1, 0, 0 });
        const float3 expected = { filler.x * 0.5f, 0, filler.z * 0.5f };
        zox_log("[tst] bed facing [%i]: filler [%i %i %i], box centre [%.1f %.1f %.1f] half [%.1f %.1f %.1f]", direction,
            filler.x, filler.y, filler.z, offset.x, offset.y, offset.z, extents.x, extents.y, extents.z);
        zox_tst_check(float3_equals(offset, expected), "bed facing [%i]: box centre [%.1f %.1f %.1f], not [%.1f %.1f %.1f]", direction,
            offset.x, offset.y, offset.z, expected.x, expected.y, expected.z);
        const byte along_x = filler.x != 0;
        zox_tst_check(extents.x == (along_x ? 1 : 0.5f) && extents.y == 0.5f && extents.z == (along_x ? 0.5f : 1),
            "bed facing [%i]: box half size [%.1f %.1f %.1f]", direction, extents.x, extents.y, extents.z);
        placement_box((int3) { 1, 2, 1 }, direction, &offset, &extents);
        zox_tst_check(float3_equals(offset, (float3) { 0, 0.5f, 0 }) && float3_equals(extents, (float3) { 0.5f, 1, 0.5f }),
            "door facing [%i]: box centre [%.1f %.1f %.1f] half [%.1f %.1f %.1f]", direction, offset.x, offset.y, offset.z,
            extents.x, extents.y, extents.z);
    }
}
