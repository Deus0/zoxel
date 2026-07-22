// material attributes
typedef struct {
    gint vertex_position;
    gint vertex_uv;
    guint camera_matrix;
    guint transform_matrix;
    guint texture;
    // guint blur_strength;
} MaterialAttributesRenderTexture;
zoxc_custom(MaterialAttributesRenderTexture);

zox_tag(RenderTextureScreen);

void define_components_render_textures(ecs* world) {
    zoxd_tag(RenderTextureScreen);
    zoxd(MaterialAttributesRenderTexture);
}
