#ifndef zoxm_jump
#define zoxm_jump

#define jump_state_none     0
#define jump_state_trigger  1
#define jump_state_active   2
#define jump_state_end   3

#define land_state_none 0
#define land_state_begin 1
#define land_state_falling 8
#define land_state_active 9 // plays sound
#define land_state_end 12

zox_tag(SilentJumper);
zoxc_byte(CanJump);
zoxc_byte(JumpState);
zoxc_byte(LandState);
zoxc_double(Jump);
zoxc_byte(FlyMode);

#include "sys/_.c"

// TODO: Falling - FallSounds, FallDamage! Count Height

static inline byte get_character_can_jump(ecs *world, entity c) {
    return zox_valid(c) && zox_has(c, CanJump) ? zox_getv(c, CanJump) : 0;
}

zox_begin_module(Jumps)
    zoxd_tag(SilentJumper);
    zoxd_byte(CanJump);
    zoxd_byte(JumpState);
    zoxd_byte(LandState);
    zoxd_double(Jump);
    zoxd_byte(FlyMode);
    zox_systems_jump(world);
zox_end_module(Jumps)

#endif