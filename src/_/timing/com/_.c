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

void zox_components_timing(ecs* world) {
    zoxd_tag(TrackMaxSystem);
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
