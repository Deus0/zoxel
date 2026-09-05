zox_tag(Character);
zox_tag(RealmCharacter);
zox_tag(CharacterGeneric);
zox_tag(CharacterUnique);
zoxc_state(GenerateCharacter);
zoxc_entities(CharacterLinks);

void define_components_characters(ecs* world) {
    zoxd_tag(Character);
    zoxd_tag(RealmCharacter);
    zoxd_tag(CharacterGeneric);
    zoxd_tag(CharacterUnique);
    zoxd_state(GenerateCharacter);
    zoxd_entities(CharacterLinks);
}
