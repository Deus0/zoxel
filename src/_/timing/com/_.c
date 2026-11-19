zoxc_byte(TimerState);
zoxc_double(DestroyInTime);
zoxc_double(TimerTime);
zoxc_double(TimerRate);
zoxc_double(SystemDelta);
zoxc_double(SystemDeltaCache);
zoxc_double(SystemDeltaMax);
zoxc_int(SystemProcessed);     // Counter for how many times it processed
zoxc_int(SystemProcessedCache);
zox_tag(TrackMaxSystem);
// a clock (sys/clock.c): a realm's, its terrain's, the things in it; paused, a realm's scale is 0 and all under it stop
zoxc_double(ClockTime);     // its seconds passed
zoxc_float(ClockDelta);     // this frame's seconds (0 paused): what its systems move by, not zox_delta_time
zoxc_float(ClockScale);     // its rate of the clock it follows (1, 0 stopped)
zox_tag(ClockLink);         // the clock it follows (a terrain its realm's, a character its terrain's), none the frame's

void zox_components_timing(ecs* world) {
    zoxd_tag(TrackMaxSystem);
    zoxd_double(ClockTime);
    zoxd_float(ClockDelta);
    zoxd_float(ClockScale);
    zoxd_nf_link(ClockLink);
    zoxd_byte(TimerState);
    zoxd_double(DestroyInTime);
    zoxd_double(TimerTime);
    zoxd_double(TimerRate);
    zoxd_double(SystemDelta);
    zoxd_double(SystemDeltaCache);
    zoxd_double(SystemDeltaMax);
    zoxd_int(SystemProcessed);
    zoxd_int(SystemProcessedCache);
}
