// Run from DialoguetreeRun: its window pops out (on_removed_dialogue_ui gives input back once it's gone), the npc
//  stops following; main thread, ui_pop_out spawns a tween
void dialogue_end_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        ui_pop_out(world, zox_get_link(world, e, DialogueUI));
        const entity speaker_b = get_speaker_b(world, e);
        if (zox_valid(speaker_b)) {
            unfollow(world, speaker_b);
        }
    }
    zox_sys_on_end();
} zoxd_system(dialogue_end_system);
