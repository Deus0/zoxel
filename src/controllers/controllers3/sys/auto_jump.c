// auto jump (setting auto_jump): a grounded player walking into a block with air over it, and over its head, jumps
#define auto_jump_ahead 0.2f    // seconds of its velocity looked ahead
#define auto_jump_min_speed 0.2f

void player_auto_jump_system(iter* it) {
    zox_sys_on_begin();
    if (!auto_jump) {
        zox_sys_on_end();
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_playing) {
            continue;
        }
        const entity character = zox_get_link(world, e, CharacterLink);
        if (!zox_valid(character) || !zox_has(character, Character3) || zox_has(character, DisableMovement) ||
            !zox_getv(character, CanJump) || zox_getv(character, Grounded) != ground_state_grounded || zox_has(character, InWater)) {
            continue;
        }
        const entity terrain = zox_get_link(world, character, TerrainLink);
        if (!zox_valid(terrain)) {
            continue;
        }
        const float3 velocity = zox_getv(character, Velocity3D);
        const float speed = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
        if (speed < auto_jump_min_speed) {
            continue;
        }
        const float scale = zox_getv(terrain, BlockScale);
        const float3 bounds = zox_getv(character, Bounds3D);
        const float3 position = zox_getv(character, Position3D);
        // just past its side, toward where it walks, at its feet (a little up, off the ground)
        const float reach = bounds.x + speed * auto_jump_ahead;
        const float3 ahead = {
            position.x + velocity.x / speed * reach,
            position.y - bounds.y + scale * 0.5f,
            position.z + velocity.z / speed * reach };
        const int3 block = real_position_to_block_position(ahead, scale);
        const entity realm = zox_get_parent(world, terrain);
        if (!block_is_solid_at(world, terrain, realm, block)) {
            continue;
        }
        // room to step up: air over the block, for its height, and over its head
        const int height = (int) ceilf(bounds.y * 2 / scale);
        byte blocked = 0;
        const int3 head = real_position_to_block_position((float3) { position.x, position.y + bounds.y, position.z }, scale);
        if (block_is_solid_at(world, terrain, realm, int3_add(head, int3_up))) {
            blocked = 1;
        }
        for (int y = 1; y <= height && !blocked; y++) {
            blocked = block_is_solid_at(world, terrain, realm, (int3) { block.x, block.y + y, block.z });
        }
        if (!blocked) {
            zox_setv(character, JumpState, jump_state_trigger);
        }
    }
    zox_sys_on_end();
} zoxd_system(player_auto_jump_system);
