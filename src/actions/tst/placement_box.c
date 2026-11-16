// tst: placement_box, the placement gizmo's box for a sized block (actions/sys/placement_gizmo.c)
//  - a bed (2 x 1 x 1) turned each way: its box over its two cells (the base and its turned filler), 2 blocks long
//    along x or z, 1 across; a door (1 x 2 x 1) any way: 1 x 2 x 1, half a block up from its base cell
//  - the box matches multi_block_place's cells (block_direction_offset)
//  - looking along x or z, a bed faces you and sticks out away (the aimed cell its near end, block_base_from_target),
//    a door faces you on the aimed cell (block_direction_from_forward)
//  - run: bash bsh/tst.sh placement_box
byte zox_tst_placement_box_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_placement_box_check(ecs* world) {
    (void) world;
    // looking along each way
    const float3 forwards[4] = { { 1, 0, 0.2f }, { 0, -0.3f, 1 }, { -1, 0.1f, 0 }, { 0.1f, 0, -1 } };
    const int3 expected_fillers[4] = { { 1, 0, 0 }, { 0, 0, 1 }, { -1, 0, 0 }, { 0, 0, -1 } };   // the way you look
    for (byte i = 0; i < 4; i++) {
        const int3 filler = block_direction_offset(block_direction_from_forward(forwards[i], (int3) { 2, 1, 1 }), (int3) { 1, 0, 0 });
        // a bed faces you (its filler from its base back towards you), and sticks out away: its base one cell past the
        //  aimed cell, so it covers the aimed cell and the one beyond
        const byte bed_direction = block_direction_from_forward(forwards[i], (int3) { 2, 1, 1 });
        zox_tst_check(int3_equals(filler, int3_scale(expected_fillers[i], -1)), "looking [%.1f %.1f %.1f], the bed's filler from its base is [%i %i %i]",
            forwards[i].x, forwards[i].y, forwards[i].z, filler.x, filler.y, filler.z);
        const int3 base = block_base_from_target(int3_zero, (int3) { 2, 1, 1 }, bed_direction);
        zox_tst_check(int3_equals(base, expected_fillers[i]) && int3_equals(int3_add(base, filler), int3_zero),
            "looking [%.1f %.1f %.1f], the bed's base is [%i %i %i] from the aimed cell, its other half [%i %i %i]", forwards[i].x,
            forwards[i].y, forwards[i].z, base.x, base.y, base.z, base.x + filler.x, base.y + filler.y, base.z + filler.z);
        zox_tst_check(int3_equals(block_base_from_target(int3_zero, (int3) { 1, 2, 1 }, bed_direction), int3_zero),
            "a door's base moved from the aimed cell");
        // a door's back (+z) that way, its front to you
        const int3 back = block_direction_offset(block_direction_from_forward(forwards[i], (int3) { 1, 2, 1 }), (int3) { 0, 0, 1 });
        zox_tst_check(int3_equals(back, expected_fillers[i]), "looking [%.1f %.1f %.1f], the door's back is [%i %i %i]",
            forwards[i].x, forwards[i].y, forwards[i].z, back.x, back.y, back.z);
    }
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

void zox_tsts_placement_box(ecs* world) {
    zox_tst_add("placement_box", zox_tst_placement_box_ready, zox_tst_placement_box_check, 30);
    zox_tst_set_setup("placement_box", NULL, 1);
}
