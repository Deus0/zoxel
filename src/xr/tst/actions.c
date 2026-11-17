// tst: xr_actions, a headset's right trigger uses the selected action (controllers3/sys/trigger.c: RT is action A),
//  as a gamepad's and a mouse's do, with the 3D action bar up
//  - in a game on the simulator, the head pitched down at the ground ahead
//  - punch selected (an action bar slot without an item): RT, the action's used (its cooldown begun)
//  - a block selected (a block item, given to the action bar: the test level starts with none): RT, the cell before
//    the aimed one turns solid
//  - run: bash bsh/tst.sh xr_actions
int zox_tst_xr_act_stage = 0;
int zox_tst_xr_act_frames = 0;
byte zox_tst_xr_act_punch_slot = 255;
byte zox_tst_xr_act_block_slot = 255;
byte zox_tst_xr_act_punched = 0;
byte zox_tst_xr_act_triggered = 0;      // TriggerActionA seen
byte zox_tst_xr_act_placed = 0;
int zox_tst_xr_act_before = -1;
int3 zox_tst_xr_act_cell;

// a realm's block item (an Item linked to a block)
static inline entity zox_tst_xr_act_block_item(ecs* world, entity realm) {
    entity found = 0;
    iter it = zox_children(world, realm);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], Item) && zox_valid(zox_get_link(world, it.entities[i], BlockLink))) {
                found = it.entities[i];
            }
        }
    }
    return found;
}

static inline byte zox_tst_xr_act_is_block(ecs* world, entity data) {
    const entity meta = zox_valid(data) ? zox_getp(world, data) : 0;
    return zox_valid(data) && (zox_valid(zox_get_link(world, data, BlockLink)) || (zox_valid(meta) && zox_valid(zox_get_link(world, meta, BlockLink))));
}

static inline void zox_tst_xr_act_next() {
    zox_tst_xr_act_stage++;
    zox_tst_xr_act_frames = 0;
}

// the action's been used: in its cooldown or warming up or active
static inline byte zox_tst_xr_act_used(ecs* world, entity action) {
    if (!zox_valid(action)) {
        return 0;
    }
    return (zox_has(action, CooldownAt) && zox_getv(action, CooldownAt) > 0) ||
        (zox_has(action, CooldownState) && zox_getv(action, CooldownState)) ||
        (zox_has(action, WarmupAt) && zox_getv(action, WarmupAt) > 0) ||
        (zox_has(action, Activate) && zox_getv(action, Activate));
}

byte zox_tst_xr_actions_ready(ecs* world) {
    if (zox_tst_xr_play_stage < 3) {
        zox_tst_xr_play_ready(world);
        return 0;
    }
    const entity character = xr_body_character(world);
    if (!zox_valid(character) || !xr_body_head(world, xr_camera)) {
        zox_tst_waiting = "the xr camera on the character's head";
        return 0;
    }
    const int f = ++zox_tst_xr_act_frames;
    zox_tst_progress(zox_tst_xr_act_stage * 1000 + f);
    xr_sim_head_euler = (float3) { -0.7f, 0, 0 };
    xr_sim_script_rt = 0;
    if (zox_has(character, TriggerActionA) && zox_getv(character, TriggerActionA)) {
        zox_tst_xr_act_triggered = 1;
    }
    const entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    entity slots[zox_children_capacity];
    const uint count = zox_valid(actionbar) ? zox_get_children_by_id(world, actionbar, slots, zox_children_capacity, zox_id(Slot)) : 0;
    switch (zox_tst_xr_act_stage) {
        case 0:
            zox_tst_waiting = "the action bar";
            if (f < 30 || !count) {
                return f > 600;
            }
            for (uint i = 0; i < count; i++) {
                const entity data = zox_get_link(world, slots[i], DataLink);
                if (!zox_valid(data)) {
                    continue;
                }
                const byte block = zox_tst_xr_act_is_block(world, data);
                if (block && zox_tst_xr_act_block_slot == 255) {
                    zox_tst_xr_act_block_slot = zox_getv(slots[i], ChildIndex);
                } else if (!block && !zox_has(data, Item) && zox_tst_xr_act_punch_slot == 255) {
                    zox_tst_xr_act_punch_slot = zox_getv(slots[i], ChildIndex);
                }
            }
            if (zox_tst_xr_act_block_slot == 255) {
                // none: one given, looked for again next frame
                static byte given = 0;
                if (!given) {
                    given = 1;
                    const entity item = zox_tst_xr_act_block_item(world, zox_get_link(world, character, RealmLink));
                    if (!zox_valid(item) || !add_item_to_slot_manager(world, actionbar, character, item, 4)) {
                        zox_tst_check(0, "no block item to give, or the action bar is full");
                        return 1;
                    }
                    zox_tst_xr_act_punch_slot = 255;
                    return 0;
                }
                if (f < 90) {
                    zox_tst_xr_act_punch_slot = 255;
                    return 0;
                }
            }
            zox_tst_xr_act_next();
            return 0;
        case 1:
            zox_tst_waiting = "punching";
            if (zox_tst_xr_act_punch_slot == 255) {
                zox_tst_xr_act_next();
                return 0;
            }
            zox_setv(character, ActionIndex, zox_tst_xr_act_punch_slot);
            xr_sim_script_rt = f >= 20 && f < 26;
            if (f > 20 && zox_tst_xr_act_used(world, zox_getv(character, ActiveAction))) {
                zox_tst_xr_act_punched = 1;
            }
            if (f > 90) {
                zox_tst_xr_act_next();
            }
            return 0;
        case 2:
            zox_tst_waiting = "placing";
            if (zox_tst_xr_act_block_slot == 255) {
                return 1;
            }
            zox_setv(character, ActionIndex, zox_tst_xr_act_block_slot);
            {
                const entity terrain = local_terrain;
                // the character's aim (its RaycastVoxelData, what placing reads: items/sys/activate.c)
                if (f == 20 && zox_has(character, RaycastVoxelData) && zox_valid(terrain)) {
                    const RaycastVoxelData* ray = zox_get(character, RaycastVoxelData);
                    // where placing puts it: the last cell's chunk and place in it (items/sys/activate.c)
                    zox_tst_xr_act_cell = zox_valid(ray->chunk_last) ? chunk_block_position(world, ray->chunk_last, ray->positionl_last) : ray->positionv_last;
                    zox_log("[tst] xr_actions: in range [%i] (distance [%.2f])", zox_has(character, RaycastRange) && ray->distance <= zox_getv(character, RaycastRange), ray->distance);
                    zox_tst_xr_act_before = ray->result ? terrain_voxel_at(world, terrain, zox_tst_xr_act_cell) : -1;
                    zox_log("[tst] xr_actions: aiming result [%i], the cell before [%i %i %i] voxel [%i]", ray->result,
                        zox_tst_xr_act_cell.x, zox_tst_xr_act_cell.y, zox_tst_xr_act_cell.z, zox_tst_xr_act_before);
                }
                xr_sim_script_rt = f >= 30 && f < 36;
                if (f == 120 && zox_valid(terrain) && zox_tst_xr_act_before == 0) {
                    zox_tst_xr_act_placed = terrain_voxel_at(world, terrain, zox_tst_xr_act_cell) > 0;
                }
            }
            if (f > 120) {
                xr_sim_head_euler = float3_zero;
                return 1;
            }
            return 0;
    }
    return 0;
}

void zox_tst_xr_actions_check(ecs* world) {
    (void) world;
    zox_log("[tst] xr_actions: punch slot [%i] block slot [%i], RT triggered [%i], punched [%i], placed [%i] (the cell was [%i])",
        zox_tst_xr_act_punch_slot, zox_tst_xr_act_block_slot, zox_tst_xr_act_triggered, zox_tst_xr_act_punched,
        zox_tst_xr_act_placed, zox_tst_xr_act_before);
    zox_tst_check(zox_tst_xr_act_triggered, "the right trigger never triggered an action");
    zox_tst_check(zox_tst_xr_act_punch_slot != 255, "no punch on the action bar");
    zox_tst_check(zox_tst_xr_act_punched, "the right trigger didn't punch");
    zox_tst_check(zox_tst_xr_act_block_slot != 255, "no block on the action bar");
    zox_tst_check(zox_tst_xr_act_before == 0, "aiming at the ground, the cell before isn't air [%i]", zox_tst_xr_act_before);
    zox_tst_check(zox_tst_xr_act_placed, "the right trigger didn't place the block");
}

void zox_tsts_xr_actions(ecs* world) {
    zox_tst_add("xr_actions", zox_tst_xr_actions_ready, zox_tst_xr_actions_check, 120);
    zox_tst_set_setup("xr_actions", zox_tst_xr_sim_setup, 1);
}
