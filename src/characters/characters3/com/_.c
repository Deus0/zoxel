zox_tag(Character3);
zoxc_byte(CharactersChanceMax);
// the move (doc/plans/movement.md): what a character wants, written by its player's input (later its ai), the move
//  (controllers3/sys/move.c) walks and turns it by
zoxc_float3(MoveDirection);     // where it wants to go, world, flat: in its own frame each axis -1 to 1 (a stick)
zoxc_float3(FaceDirection);     // the way it wants to face, world, flat; zero: no turn wanted (first person: the mouse)
zox_tag(Running);               // running while on (non fragmenting)
// zoxc_byte(Character3Type);

void zox_components_characters3(ecs* world) {
    zoxd_tag(Character3);
    zoxd_byte(CharactersChanceMax);
    zoxd_float3(MoveDirection);
    zoxd_float3(FaceDirection);
    zoxd_nf_tag(Running);
    // zoxd_byte(Character3Type);
}
