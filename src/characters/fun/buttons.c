void button_event_jump(ecs *world, ClickEventData event) {
    zox_geter_value(event.clicker, CharacterLink, entity, e);
    if (zox_valid(e) && zox_getv(e, CanJump)) {
        zox_setv(e, JumpState, 1);
    }
}

void button_event_attack(ecs *world, ClickEventData event) {
    entity character = zox_getv(event.clicker, CharacterLink);
    if (character) {
        zox_setv(character, TriggerActionA, 1);
    }
}

void button_event_speak(ecs *world, ClickEventData event) {
    entity character = zox_getv(event.clicker, CharacterLink);
    if (character) {
        zox_setv(character, TriggerActionB, 1);
    }
}