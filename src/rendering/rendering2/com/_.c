zox_tag(BasicMesh2);
zox_tag(TexturedMesh2);
zoxc_guint(MaterialBlur);
zoxc_guint(MaterialVignette);
zoxc_guint(MaterialTint);
#include "matrix.c"
#include "textured.c"

void zox_components_basics2(ecs* world) {
    zoxd_tag(BasicMesh2);
    zoxd_tag(TexturedMesh2);
    zoxd_guint(MaterialBlur);
    zoxd_guint(MaterialVignette);
    zoxd_guint(MaterialTint);
    zoxd(MaterialTextured2D);
    zoxd(attributes_matrixui);
}
