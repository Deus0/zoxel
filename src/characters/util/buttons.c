/* inline byte can_jump(ecs_world_t *world, const ecs_entity_t e) {
    return !zox_gett_value(e, DisableMovement) && zox_gett_value(e, Grounded);
}*/

void button_event_jump(ecs_world_t *world, const ClickEventData *event) {
    zox_geter_value(event->clicker, CharacterLink, entity, e);
    if (zox_valid(e) && zox_gett_value(e, CanJump)) {
        zox_set(e, JumpState, { zox_dirty_trigger });
    }
}

void button_event_attack(ecs_world_t *world, const ClickEventData *event) {
    const ecs_entity_t character = zox_get_value(event->clicker, CharacterLink)
    if (character) {
        zox_set(character, TriggerActionB, { 1 })
    }
}