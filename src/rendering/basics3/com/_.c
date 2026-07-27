zox_tag(MeshBasic3D);
zox_tag(TexturedMesh3D);
zox_tag(SkeletonMesh);

void zox_define_components_basics3(ecs* world) {
    zoxd_tag(MeshBasic3D);
    zoxd_tag(TexturedMesh3D);
    zoxd_tag(SkeletonMesh);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialBasic3D);
}
