zox_tag(Character);
zox_tag(CharacterGeneric);
zox_tag(CharacterUnique);
zoxc_state(GenerateCharacter);
zox_tag(CharacterLink);
zoxc_entities(CharacterLinks);

void zox_components_characters(ecs* world) {
    zoxd_tag(Character);
    zoxd_tag(CharacterGeneric);
    zoxd_tag(CharacterUnique);
    zoxd_state(GenerateCharacter);
    zoxd_nf_tag(CharacterLink);
    zoxd_entities(CharacterLinks);
}
