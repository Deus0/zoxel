zoxc_byte(TimerState);
zoxc_double(DestroyInTime);
zoxc_double(TimerTime);
zoxc_double(TimerRate);
zoxc_double(SystemDelta);
zoxc_double(SystemDeltaCache);
zoxc_double(SystemDeltaMax);
zoxc_int(SystemProcessed);     // Counter for how many times it processed
zoxc_int(SystemProcessedCache);

void define_components_timing(ecs* world) {
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