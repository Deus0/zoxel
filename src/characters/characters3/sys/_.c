#include "saving.c"
#include "render_depth.c"
#include "debug_types.c"
#include "realm.c"
realm_clear_system(CharacterLinks);

void define_systems_characters3(ecs *world) {
    realm_clear_systemd(characters, CharacterLinks);
    zox_system(
        CharacterRenderDepthSystem,
        EcsOnUpdate,
        [in] rendering.RenderDepthDirty,
        [out] chunks3.BuildChunkMesh,
        [none] Character3
    );
    zox_system(
        CharacterSaveSystem,
        EcsOnStore,
        [in] realms.RealmLink,
        [in] transforms3.Position3D,
        [in] transforms3.Euler,
        [out] saves.SaveHash,
        [none] saves.Saver
    );
}

/*zox_system(
    Character3TypeDebugSystem,
    EcsOnUpdate,
    [in] lines3.DebugCubeLines,
    [in] characters3.Character3Type,
    [out] colorz.Color,
    [none] Character3
);*/
