// a character asleep in a bed (fun/_.c)
zox_tag(Sleeping);          // asleep: on_sleeping lays it on its bed
zox_tag(Waking);            // waking up (a one-off event): on_waking stands it up, removes it and Sleeping
zox_tag(SleepUI);           // a player's window while asleep, its "Wake" button
zoxc_int3(SleepBlock);      // its bed's base block

void zox_components_beds(ecs* world) {
    zoxd_tag(Sleeping);
    zoxd_nf_tag(Waking);
    zoxd_tag(SleepUI);
    zoxd_int3(SleepBlock);
}
