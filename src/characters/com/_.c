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
// who a character is (its id window, stats/ui/ins/id.c), each links to its data (races, classes, jobs, clans modules),
//  none until they're made
zox_tag(RaceLink);
zox_tag(ClassLink);
zox_tag(JobLink);          // a job (jrpg style: what it does, apart from its class)
zox_tag(ClanLink);
zox_tag(TitleLink);

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
    zoxd_nf_link(RaceLink);
    zoxd_nf_link(ClassLink);
    zoxd_nf_link(JobLink);
    zoxd_nf_link(ClanLink);
    zoxd_nf_link(TitleLink);
}
