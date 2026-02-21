#include "saving.c"
#include "render_depth.c"
#include "debug_types.c"
#include "realm.c"
#include "name_labels.c"

realm_clear_system(CharacterLinks);

void define_systems_characters3(ecs *world) {
    realm_clear_systemd(characters, CharacterLinks);

    zox_system_1(
        Character3RealmSpawnSystem,
        EcsOnLoad,
        [in] realms.GenerateRealm,
        [in] rendering.ModelLinks,
        [out] characters.CharacterLinks,
        [out] characters3.CharactersChanceMax,
        [none] realms.Realm
    );

    zox_system(
        CharacterSaveSystem,
        EcsOnStore,
        [in] realms.RealmLink,
        [in] transforms3.Position3D,
        [in] transforms3.Euler,
        [out] characters3.CharacterSaveHash,
        [none] characters3.SaveCharacter
    );

    zox_system(
        CharacterRenderDepthSystem,
        EcsOnUpdate,
        [in] rendering.RenderDepthDirty,
        [out] chunks3.ChunkMeshDirty,
        [none] Character3
    );

    zox_system(
        Character3TypeDebugSystem,
        EcsOnUpdate,
        [in] lines3.DebugCubeLines,
        [in] characters3.Character3Type,
        [out] colorz.Color,
        [none] Character3
    );
}
