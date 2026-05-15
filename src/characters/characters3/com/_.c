zox_tag(Character3);
zox_tag(SaveCharacter);
zoxc_byte(CharactersChanceMax);
zoxc_byte(Character3Type);
zoxc_ulong(CharacterSaveHash);
zoxc_entity(Character3Link);
zoxc_entity(CharacterMetaLink);

void define_components_characters3(ecs* world) {
    zoxd_tag(Character3);
    zoxd_tag(SaveCharacter);
    zoxd_byte(CharactersChanceMax);
    zoxd_byte(Character3Type);
    zoxd_ulong(CharacterSaveHash);
    zoxd_entity(Character3Link);
    zoxd_entity(CharacterMetaLink);
}
