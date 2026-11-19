// a character asleep in a bed (fun/_.c)
zox_tag(Sleeping);          // asleep: on_sleeping lays it on its bed
zox_tag(Waking);            // waking up (a one-off event): on_waking stands it up, removes it and Sleeping
zox_tag(SleepUI);           // a player's window while asleep, its "Wake" button
zoxc_int3(SleepBlock);      // its bed's base block
// getting onto a bed and lying down, or up and off it (sleep_motion_system): from, through the middle (standing on
//  the mattress), to; started at start (zox_current_time)
typedef struct {
    float3 positions[3];
    float4 rotations[3];
    double start;
} SleepPath;
zoxc(SleepMotion, SleepPath);

void zox_components_beds(ecs* world) {
    zoxd_tag(Sleeping);
    zoxd_nf_tag(Waking);
    zoxd_tag(SleepUI);
    zoxd_int3(SleepBlock);
    zoxd(SleepMotion);
}
