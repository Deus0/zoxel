#include "saving.c"
#include "render_depth.c"
#include "debug_types.c"
#include "realm.c"

void define_systems_characters3(ecs *world) {
    zox_system(
        CharacterRenderDepthSystem,
        zoxp_update,
        [in] rendering.RenderDepthDirty,
        [none] Character3
    );
    zox_system(
        CharacterSaveSystem,
        zoxp_update,
        [in] realms.RealmLink,
        [in] transforms3.Position3D,
        [in] transforms3.Euler,
        [out] saves.SaveHash,
        [none] saves.Saver
    );
}

/*zox_system(
    Character3TypeDebugSystem,
    zoxp_update,
    [in] lines3.DebugCubeLines,
    [in] characters3.Character3Type,
    [out] colorz.Color,
    [none] Character3
);*/
