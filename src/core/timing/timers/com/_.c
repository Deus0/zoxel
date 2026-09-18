zoxc_byte(CanActivate);     // stats resource will validate this
zoxc_double(WarmupAt);
zoxc_double(CooldownAt);
zoxc_double(WarmupTime);
zoxc_double(CooldownTime);
zoxc_double(ActivateTime);
zoxc_state(ActivateBegin);
zoxc_state(WarmupState);
zoxc_state(Activate);
zoxc_state(CooldownState);

void zox_components_timers(ecs* world) {
    zoxd_byte(CanActivate);
    zoxd_double(WarmupTime);
    zoxd_double(CooldownTime);
    zoxd_double(ActivateTime);
    zoxd_double(WarmupAt);
    zoxd_double(CooldownAt);
    zoxd_state(ActivateBegin);
    zoxd_state(Activate);
    zoxd_state(WarmupState);
    zoxd_state(CooldownState);
}
