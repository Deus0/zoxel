// a dialogue ending (its process deleted): its speakers look ahead again
void on_removed_dialogue_process(iter* it) {
    ecs* world = it->world;
    for (int i = 0; i < it->count; i++) {
        const entity e = it->entities[i];
        zox_each_link(world, e, SpeakerLinks, speaker) {
            if (zox_valid(speaker)) {
                look_at(world, speaker, 0);
            }
        }
    }
}
