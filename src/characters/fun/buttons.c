void button_event_jump(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity character = zox_get_link(world, player, Character);
    if (zox_valid(character) && zox_getv(character, CanJump)) {
        zox_setv(character, JumpState, 1);
    }
}

void button_event_attack(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity character = zox_get_link(world, player, Character);
    if (character) {
        zox_setv(character, TriggerActionA, 1);
    }
}

void button_event_speak(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity character = zox_get_link(world, player, Character);
    if (character) {
        zox_setv(character, TriggerActionB, 1);
    }
}
