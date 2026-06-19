#include "melee.c"
#include "dots.c"
#include "activate.c"
#include "toggle.c"
#include "character.c"
#include "block_damage.c"
realm_clear_system(SkillLinks);
// TODO: DotLinks just parent them instead

void zox_define_systems_skills(ecs *world) {
    realm_clear_systemd(skills, SkillLinks);
    zox_system(
        SkillToggleSystem,
        EcsOnUpdate,
        [in] timers.Activate,
        [out] skills.SkillActive,
        [none] skills.SkillToggleable
    );
    zox_system(
        DotsSystem,
        EcsOnUpdate,
        [in] skills.SpawnerLink,
        [in] skills.SkillDamage,
        [none] skills.Dot
    );
    // TODO: split into sound, resource and damage systems
    zox_system_1(
        MeleeSystem,
        zoxp_queue_add,
        [in] skills.SkillDamage,
        [in] skills.SkillDamageMax,
        [in] skills.SkillRange,
        [in] skills.SkillResourceLink,
        [in] skills.SkillCost,
        [in] timers.Activate,
        [none] skills.Melee
    );
    /*zox_system_1(
        CharacterSkillsSpawnSystem,
        EcsOnUpdate,
        [in] characters.GenerateCharacter,
        [in] realms.RealmLink,
        [out] skills.SkillLinks
        // [none] !players.PlayerLink
    );*/
    zox_system_1(
        BlockDamageQueueSystem,
        zoxp_mainthread,
        [in] chunks3.ChunkPosition,
        [in] chunks.NodeDepth,
        [out] blocks.BlockDamageQueue,
        [none] chunks3.Chunk3
    );

}
