void random_jump3d_system(iter* it) {
    zox_sys_on_begin();
    const int random_jump_chance = 9988;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CanJump);
    zox_sys_out(JumpState);
    zox_sys_in(ClockDelta);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CanJump, canJump);
        zox_sys_o(JumpState, jump);
        // its clock stopped (paused): no jumps
        if (jump->value || (ClockDelta_ && ClockDelta_[i].value <= 0)) {
            continue;
        }
        // stays still while talking (its head looks at a speaker)
        // todo: unless it's hyper, once characters have temperaments
        if (zox_valid(looking_at(world, e))) {
            continue;
        }
        if (canJump->value) {
            if (rand() % 10000 >= random_jump_chance) {
                jump->value = zox_dirty_trigger;
            }
        }
    } 
    zox_sys_on_end();
} zoxd_system(random_jump3d_system);