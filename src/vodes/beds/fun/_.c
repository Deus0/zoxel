// Beds: sleeping in one, waking up
#define sleep_lids_close 2.0f       // seconds for the eyes to close (from the start of the climb)
#define sleep_lids_open 0.8f        // and to open
#define sleep_climb_time 1.2f       // seconds to climb on and lie down, or to get up and off: half each
#define sleep_climb_hop 0.3f        // of a block, the hop up onto the mattress
#define bed_mattress_height 0.5f    // of a block: the mattress's top (models/gen/furniture.c, 8 of 16)
const char* sleep_ui_header = "Asleep";
const char* sleep_ui_wake = "Wake";
const char* sleep_respawn_set = "Respawn point set";

// a bed's direction, its base's block data
static inline byte bed_direction(ecs* world, entity terrain, int3 base) {
    return block_data_direction(terrain_block_data_at(world, terrain, base));
}

// lying on a bed: its middle, on the mattress; its up to the pillow (the base's end), its front to the sky
//  - half_depth: the body's half thickness (front to back), it lies on its back that far over the mattress
static inline void bed_lie_pose(byte direction, int3 base, float block, float half_depth, float3* position, float4* rotation) {
    const int3 along = block_direction_offset(direction, (int3) { 1, 0, 0 });   // from its base to its other half
    const float3 head = int3_to_float3(int3_scale(along, -1));
    *position = (float3) {
        (base.x + 0.5f + 0.5f * along.x) * block,
        (base.y + bed_mattress_height) * block + half_depth,
        (base.z + 0.5f + 0.5f * along.z) * block };
    // on its back: its front (+z) turned to the sky, then about up so its up (now along -z) goes to the pillow
    const float4 back = quaternion_from_to((float3) { 0, 0, 1 }, (float3) { 0, 1, 0 });
    const float3 up = float4_rotate_float3(back, (float3) { 0, 1, 0 });
    const float dot = up.x * head.x + up.z * head.z;
    const float4 turn = dot < -0.99f ? quaternion_from_axis_angle((float3) { 0, 1, 0 }, (float) M_PI) :
        quaternion_from_to(up, head);
    *rotation = quaternion_rotate(turn, back);
}

// getting up: beside the bed's base (its front, else its back), two blocks of air, ground under it first; on the bed
//  if both sides are blocked
static inline void bed_stand_pose(ecs* world, entity terrain, entity realm, byte direction, int3 base, float block,
    float half_height, float3* position, float4* rotation) {
    int3 stand = { base.x, base.y + 1, base.z };
    byte found = 0;
    for (byte grounded = 1; !found && grounded < 2; grounded--) {
        for (int side = -1; side <= 1 && !found; side += 2) {
            const int3 at = int3_add(base, block_direction_offset(direction, (int3) { 0, 0, side }));
            if (!terrain_voxel_at(world, terrain, at) && !terrain_voxel_at(world, terrain, (int3) { at.x, at.y + 1, at.z }) &&
                (!grounded || block_is_solid_at(world, terrain, realm, (int3) { at.x, at.y - 1, at.z }))) {
                stand = at;
                found = 1;
            }
        }
    }
    *position = (float3) { (stand.x + 0.5f) * block, stand.y * block + half_height, (stand.z + 0.5f) * block };
    *rotation = block_direction_rotation(direction);
}

// standing on the bed's mattress, over its middle (the climb's middle)
static inline float3 bed_middle_standing(byte direction, int3 base, float block, float half_height) {
    const int3 along = block_direction_offset(direction, (int3) { 1, 0, 0 });
    return (float3) { (base.x + 0.5f + 0.5f * along.x) * block, (base.y + bed_mattress_height) * block + half_height,
        (base.z + 0.5f + 0.5f * along.z) * block };
}

// a pose along a path at t (0 to 1): its first half to the middle (a hop up), its second on, each eased
static inline void sleep_path_at(const SleepPath* path, float t, float block, float3* position, float4* rotation) {
    const byte second = t >= 0.5f;
    float u = second ? (t - 0.5f) * 2 : t * 2;
    u = u * u * (3 - 2 * u);
    *position = float3_lerp(path->positions[second], path->positions[second + 1], u);
    *rotation = quaternion_slerp(path->rotations[second], path->rotations[second + 1], u);
    if (!second) {
        position->y += sinf(u * (float) M_PI) * sleep_climb_hop * block;
    }
}

// puts a character to sleep in the bed at base (from a worker too: deferred), on_sleeping does the rest
void character_sleep(ecs* world, entity character, int3 base) {
    if (zox_has(character, Sleeping)) {
        return;
    }
    zox_set(character, SleepBlock, { base });
    zox_add(character, Sleeping);
}

// wakes a sleeping character, on_waking does the rest
void character_wake(ecs* world, entity character) {
    if (zox_has(character, Sleeping)) {
        zox_add(character, Waking);
    }
}

void button_event_wake(ecs* world, ClickEventData event) {
    character_wake(world, zox_get_link(world, event.clicker, CharacterLink));
}

static inline void sleep_mouse_lock(ecs* world, entity player, byte lock) {
    const entity mouse = zox_get_child_by_id(world, player, zox_id(Mouse));
    if (zox_valid(mouse)) {
        zox_setv(mouse, MouseLock, lock);
    }
}

static inline entity spawn_sleep_ui(ecs* world, entity player) {
    SpawnListElement elements[1] = { { .text = sleep_ui_wake, .on_click = { &button_event_wake } } };
    const entity e = spawn_window_list(world, prefab_window, player, sleep_ui_header,
        zox_huds_element_font_size * ui_scale, zox_huds_element_font_size * ui_scale, (ClickEvent) { NULL }, 0, 0, 0,
        zox_huds_window_alignment, zox_huds_window_anchor, byte2_single(zox_huds_list_padding * ui_scale), NULL,
        elements, 1, 1).x;
    zox_set_unique_name(e, "sleep_ui");
    zox_add(e, SleepUI);
    return e;
}

// fell asleep (Sleeping added): climbs on and lies down (SleepMotion), still; a player's eyes close, its mouse let go,
//  the "Wake" window
void on_sleeping(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (!zox_valid(terrain) || !zox_has(e, SleepBlock)) {
            zox_remove(e, Sleeping);
            continue;
        }
        const int3 base = zox_getv(e, SleepBlock);
        const float block = zox_getv(terrain, BlockScale);
        const float3 bounds = zox_has(e, Bounds3D) ? zox_getv(e, Bounds3D) : float3_single(0.5f * block);
        const byte direction = bed_direction(world, terrain, base);
        // from where it stands, up onto the mattress (still facing its way), then down onto its back
        SleepPath path = { .start = zox_current_time };
        path.positions[0] = zox_getv(e, Position3D);
        path.rotations[0] = zox_getv(e, Rotation3D);
        path.positions[1] = bed_middle_standing(direction, base, block, bounds.y);
        path.rotations[1] = path.rotations[0];
        bed_lie_pose(direction, base, block, bounds.z, &path.positions[2], &path.rotations[2]);
        zox_set(e, SleepMotion, { path });
        zox_add(e, DisableMovement);
        zox_add(e, DisableGravity);
        zox_add(e, NoClip);
        // and the forces from last frame (a swim stroke lands a frame later)
        if (zox_has(e, Velocity3D)) {
            zox_setv(e, Velocity3D, float3_zero);
        }
        if (zox_has(e, Acceleration3D)) {
            zox_setv(e, Acceleration3D, float3_zero);
        }
        if (zox_has(e, Omega3D)) {
            zox_setv(e, Omega3D, float3_zero);
        }
        const entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(player)) {
            continue;
        }
        const entity camera = zox_get_link(world, player, CameraLink);
        if (zox_valid(camera) && zox_has(camera, CameraLids)) {
            zox_lerp_float(camera, CameraLids, 1, sleep_lids_close);
        }
        // its respawn point: beside the bed, where it gets up (a toast when it moves here)
        float3 stand;
        float4 facing;
        bed_stand_pose(world, terrain, zox_get_parent(world, terrain), direction, base, block, bounds.y, &stand, &facing);
        if (!zox_has(player, HomePosition) || float3_distance(zox_getv(player, HomePosition), stand) > 0.5f * block) {
            zox_set(player, HomePosition, { stand });
            spawn_toast(world, zox_get_link(world, player, CanvasLink), sleep_respawn_set);
        }
        sleep_mouse_lock(world, player, 0);
        spawn_sleep_ui(world, player);
        if (zox_getv(player, PlayerState) == zox_player_state_playing) {
            zox_setv(player, PlayerState, zox_player_state_sleeping);   // its ui navigated, its controls off
        }
    }
}

// woke up (Waking added): gets up off its bed (sleep_motion_system, it moves again once off); a player's eyes open,
//  its mouse taken, the window gone
void on_waking(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_remove(e, Waking);
        zox_remove(e, Sleeping);
        const entity terrain = zox_get_link(world, e, TerrainLink);
        if (zox_valid(terrain) && zox_has(e, SleepBlock)) {
            const int3 base = zox_getv(e, SleepBlock);
            const float block = zox_getv(terrain, BlockScale);
            const float3 bounds = zox_has(e, Bounds3D) ? zox_getv(e, Bounds3D) : float3_single(0.5f * block);
            const byte direction = bed_direction(world, terrain, base);
            // up off its back onto its feet on the mattress, facing where it'll step, then down beside the bed
            SleepPath path = { .start = zox_current_time };
            path.positions[0] = zox_getv(e, Position3D);
            path.rotations[0] = zox_getv(e, Rotation3D);
            bed_stand_pose(world, terrain, zox_get_parent(world, terrain), direction, base, block, bounds.y,
                &path.positions[2], &path.rotations[2]);
            path.positions[1] = bed_middle_standing(direction, base, block, bounds.y);
            path.rotations[1] = path.rotations[2];
            zox_set(e, SleepMotion, { path });
        } else {
            zox_remove(e, DisableMovement);
            zox_remove(e, DisableGravity);
            zox_remove(e, NoClip);
        }
        const entity player = zox_get_link(world, e, PlayerLink);
        if (!zox_valid(player)) {
            continue;
        }
        const entity camera = zox_get_link(world, player, CameraLink);
        if (zox_valid(camera) && zox_has(camera, CameraLids)) {
            zox_lerp_float(camera, CameraLids, 0, sleep_lids_open);
        }
        sleep_mouse_lock(world, player, 1);
        if (zox_getv(player, PlayerState) == zox_player_state_sleeping) {
            zox_setv(player, PlayerState, zox_player_state_play_trigger);   // as a dialogue ends
            zox_setv(player, PlayerStateDirty, zox_dirty_trigger);
        }
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity ui = zox_valid(canvas) ? zox_get_child_by_id(world, canvas, zox_id(SleepUI)) : 0;
        if (zox_valid(ui)) {
            zox_delete(ui);
        }
    }
}

// died asleep: it wakes (its window gone, its player state back to play), its respawn as any death's
void on_sleeper_died(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        character_wake(world, e);
    }
}

// NOTE: Moves a sleeper along its climb (SleepMotion), on and lying down or up and off, and holds it lying while asleep
//  - after the physics (zoxp_physics, registered after it): nothing nudges it; off the bed it moves again
void sleep_motion_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SleepMotion);
    zox_sys_out(Position3D);
    zox_sys_out(Rotation3D);
    zox_sys_out(Velocity3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SleepMotion, motion);
        zox_sys_o(Position3D, position);
        zox_sys_o(Rotation3D, rotation);
        // optional: a body without physics (tst block_activate's clicker)
        Velocity3D* velocity = Velocity3D_ ? &Velocity3D_[i] : NULL;
        const entity terrain = zox_get_link(world, e, TerrainLink);
        const float block = zox_valid(terrain) ? zox_getv(terrain, BlockScale) : 1;
        const float t = float_min(1, (float) (zox_current_time - motion->value.start) / sleep_climb_time);
        sleep_path_at(&motion->value, t, block, &position->value, &rotation->value);
        if (velocity) {
            velocity->value = float3_zero;
        }
        // off the bed: moving again
        if (t >= 1 && !zox_has(e, Sleeping)) {
            zox_remove(e, SleepMotion);
            zox_remove(e, DisableMovement);
            zox_remove(e, DisableGravity);
            zox_remove(e, NoClip);
        }
    }
    zox_sys_on_end();
} zoxd_system(sleep_motion_system);
