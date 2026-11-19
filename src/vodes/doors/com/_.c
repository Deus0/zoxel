// a door's block vox (fun/_.c door_on_spawned): where it rests closed, its hinge, and how far it's swung
zoxc_byte(DoorOpen);            // 1 open, 0 closed: what it swings to (its base's block_data_open)
zoxc_float(DoorSwing);          // 0 closed to 1 open, eased to DoorOpen (sys/swing.c)
zoxc_float3(DoorRestPosition);  // its centre closed
zoxc_float4(DoorRestRotation);  // its turn closed (its direction)
zoxc_float3(DoorHinge);         // its hinge from its centre, in its own frame
zox_tag(DoorSound);             // toggled (door_toggle, maybe on a worker): on_door_sound plays its creak or thump, removes it

void zox_components_doors(ecs* world) {
    zoxd_byte(DoorOpen);
    zoxd_float(DoorSwing);
    zoxd_float3(DoorRestPosition);
    zoxd_float4(DoorRestRotation);
    zoxd_float3(DoorHinge);
    zoxd_nf_tag(DoorSound);
}
