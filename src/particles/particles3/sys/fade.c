// a particle's fade (animations' fadeout_system): as long as its life, from when its life is set (spawned, or a
//  spawner giving it its own: DestroyInTime's OnSet; its countdown writes in place, not a set)
void particle3_fade_on_life(iter* it) {
    const double now = zox_current_time;
    zox_sys_begin();
    zox_sys_in(DestroyInTime);
    zox_sys_out(FadeOutTime);
    zox_sys_out(AnimationStart);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(DestroyInTime, life);
        zox_sys_o(FadeOutTime, fade);
        zox_sys_o(AnimationStart, start);
        fade->value = life->value > 0 ? life->value : 0.01f;
        start->value = now;
    }
}
