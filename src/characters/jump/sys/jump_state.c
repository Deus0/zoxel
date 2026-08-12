// #define jump_cooldown_state 253
// #define cooldown_start_landing 251
// byte can_jump_delay = 15;

/*zox_sys2(CanJumpSystem) {
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_in(JumpState);
    zox_sys_out(CanJump);
    zox_sys_out(LandState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Grounded, grounded);
        zox_sys_i(JumpState, jump);
        zox_sys_o(CanJump, can_jump);
        zox_sys_o(LandState, landed);
        // Started to jump!
        if (jump->value) {
            can_jump->value = 0;
        }
        can_jump->value = grounded->value;
        // assume this is when landed
        if (jump->value == zox_dirty_active) {
            landed->value = cooldown_start_landing;   // waiting
            // zox_log("JUMPED - landed [%i]", landed->value);
        }
        if (grounded->value) { // && can_jump->value && can_jump->value != 255) {
            if (landed->value == 255) {
                landed->value = zox_dirty_trigger;
                // zox_log("LANDED - landed [%i]", landed->value);
            } else if (landed->value > zox_dirty_end) {
                landed->value++;
            }
        }
    }
} zox_sys_end(CanJumpSystem);

zox_sys2(JumpBeginSystem) {
    zox_sys_begin();
    zox_sys_in(JumpState);
    zox_sys_out(Jump);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(JumpState, jumpState);
        zox_sys_o(Jump, jump);
        if (jumpState->value == zox_dirty_active) {
            // zox_log("Jump Begin Truly.");
            jump->value = jump_timing;
        }
    }
} zox_sys_end(JumpBeginSystem);*/

double zox_jump_time = 0.06;
zox_sys2(JumpSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Grounded);
    zox_sys_out(JumpState);
    zox_sys_out(CanJump);
    zox_sys_out(LandState);
    zox_sys_out(Jump);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Grounded, grounded);
        zox_sys_o(JumpState, jump_state);
        zox_sys_o(CanJump, can_jump);
        zox_sys_o(LandState, landed);
        zox_sys_o(Jump, jump);
        // zox_log("Jumper [%s]: Grounded [%i] JumpState [%i] CanJump [%i] LandState [%i]", zox_getn(e), grounded->value, jump_state->value, can_jump->value, landed->value);
        // Jump trigger
        if (jump_state->value == jump_state_trigger) {
            if (grounded->value) {
                jump_state->value = jump_state_active;
                can_jump->value = 0;
                landed->value = land_state_begin;
                jump->value = zox_jump_time;
                if (dbg_log) {
                    zox_log("Jump Begins for [%s]", zox_getn(e));
                }
            } else {
                jump_state->value = jump_state_none;
            }
        }
        // Active only lasts for one frame
        else if (jump_state->value == jump_state_active) {
            jump_state->value = jump_state_none;
        }
        // Can jump whenever grounded and not currently jumping
        can_jump->value = grounded->value &&
            jump_state->value == jump_state_none &&
            landed->value == land_state_none;
        // ### LANDINGS ###
        // Landed End
        if (landed->value == land_state_end) {
            landed->value = 0;
            if (dbg_log) {
                zox_log("Landing Ended [%s]", zox_getn(e));
            }
        }
        // Landed cooldown
        if (landed->value >= land_state_active &&
            landed->value < land_state_end
        ) {
            landed->value++;
            if (dbg_log) {
                zox_log("Landed [%s]", zox_getn(e));
            }
        }
        // Landing
        if (grounded->value && landed->value == land_state_falling) {
            landed->value = land_state_active;
            if (dbg_log) {
                zox_log("Land Begins for [%s]", zox_getn(e));
            }
        }
        // Landing cooldown
        if (landed->value >= land_state_begin && landed->value < land_state_falling) {
            landed->value++;
        }
    }
} zox_sys_end(JumpSystem);