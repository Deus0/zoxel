zox_tag(Character);
zox_tag(CharacterUnique);
zoxc_state(GenerateCharacter);
zox_tag(CharacterLink);
#define zox_realm_characters_max 64
zox_tag(CharacterLinks);   // links to many character metas, i.e. from a realm
zox_tag(CharacterSlime);   // a slime character meta
zox_tag(Villager);         // a character meta living in towns, spawns most there (npcs/fun/population.c)
zox_tag(Monster);          // a character meta of the wild, spawns little in towns (npcs/fun/population.c)
zox_tag(Critter);          // a character meta of animals (chickens, penguins), spawns the same anywhere

void zox_components_characters(ecs* world) {
    zoxd_tag(Character);
    zoxd_tag(CharacterUnique);
    zoxd_state(GenerateCharacter);
    zoxd_nf_link(CharacterLink);
    zoxd_nf_links(CharacterLinks);
    zoxd_tag(CharacterSlime);
    zoxd_tag(Villager);
    zoxd_tag(Monster);
    zoxd_tag(Critter);
}
