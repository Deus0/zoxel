void button_event_jump(ecs *world, ClickEventData event) {
    zox_geter_value(event.clicker, CharacterLink, entity, e);
    if (zox_valid(e) && zox_gett_value(e, CanJump)) {
        zox_set(e, JumpState, { zox_dirty_trigger });
    }
}

void button_event_attack(ecs *world, ClickEventData event) {
    entity character = zox_getv(event.clicker, CharacterLink);
    if (character) {
        zox_set(character, TriggerActionA, { 1 });
    }
}