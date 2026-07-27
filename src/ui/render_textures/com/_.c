zox_tag(RenderTextureScreen);
// material attributes
typedef struct {
    gint vertex_position;
    gint vertex_uv;
    gint camera_matrix;
    gint transform_matrix;
    gint texture;
} MaterialAttributesRenderTexture;
zoxc_custom(MaterialAttributesRenderTexture);

void define_components_render_textures(ecs* world) {
    zoxd_tag(RenderTextureScreen);
    zoxd(MaterialAttributesRenderTexture);
}
