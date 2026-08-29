zox_tag(Mesh3);
zox_tag(MeshBasic3D);
zox_tag(TexturedMesh3);
zox_tag(SkeletonMesh);

void zox_components_rendering3(ecs* world) {
    zoxd_nf_tag(Mesh3);
    zoxd_nf_tag(MeshBasic3D);
    zoxd_nf_tag(TexturedMesh3);
    zoxd_nf_tag(SkeletonMesh);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialBasic3D);
}
