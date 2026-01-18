#include "saving.c"
#include "render_depth.c"
#include "debug_types.c"
#include "realm.c"
#include "name_labels.c"
zox_declare_system_state_event(RealmCharacters, GenerateRealm, zox_generate_realm_characters, spawn_realm_characters)

void define_systems_characters3(ecs *world) {
    zox_define_system_state_event_1(RealmCharacters, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm)
    zox_system(
        CharacterSaveSystem,
        EcsOnUpdate,
        [in] realms.RealmLink,
        [in] transforms3.Position3D,
        [in] transforms3.Euler,
        [out] CharacterSaveHash,
        [none] SaveCharacter
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