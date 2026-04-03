zox_tag(Character);
zoxc_state(GenerateCharacter);
zoxc_entity(CharacterLink);
zoxc_entities(CharacterLinks)

void define_components_characters(ecs* world) {
    zoxd_tag(Character);
    zoxd_state(GenerateCharacter);
    zoxd_entity(CharacterLink);
    zoxd_entities(CharacterLinks);
}
