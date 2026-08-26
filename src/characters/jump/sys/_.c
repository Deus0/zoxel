#include "jump_state.c"
#include "force.c"
#include "jump_begin.c"
#include "jump_sound.c"
#include "land_sound.c"
// zox_increment_system_with_reset(JumpState, zox_dirty_end);
// zox_increment_system_with_reset(LandState, zox_dirty_end);

void define_systems_jump(ecs* world) {
    // zoxd_system_increment(JumpState);
    // zoxd_system_increment(LandState);
    // jump
    // move these to Jump module? under Characters
    zox_system(
        JumpSystem,
        zoxp_update,
        [in] physics.Grounded,
        [out] jumps.JumpState,
        [out] jumps.CanJump,
        [out] jumps.LandState,
        [out] jumps.Jump,
     );
    zox_system(
        JumpForceSystem,
        zoxp_physics,
        // [in] physics3.Velocity3D,
        [out] physics3.Acceleration3D,
        [out] jumps.Jump,
        [none] !physics.DisableMovement
    );
    zox_system_1(
        JumpSoundSystem,
        zoxp_spawn,
        [in] jumps.JumpState,
        [none] !jumps.SilentJumper
    );
    zox_system_1(
        LandSoundSystem,
        zoxp_spawn,
        [in] jumps.LandState,
        [none] !jumps.SilentJumper
    );
    /*zox_system(
        CanJumpSystem,
        zoxp_update,
        [in] physics.Grounded,
        [in] jumps.JumpState,
        [out] jumps.CanJump,
        [out] jumps.LandState
     );
    zox_system(
        JumpBeginSystem,
        zoxp_update,
        [in] jumps.JumpState,
        [out] jumps.Jump
    );*/
    /*zox_system(
        Jump3VelSystem,
        zoxp_physics + 1,
        [out] physics3.Velocity3D,
        [out] jumps.Jump,
        [none] !physics.DisableMovement
    );*/
}