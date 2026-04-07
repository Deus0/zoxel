// material attributes
typedef struct {
    GLint vertex_position;
    GLint vertex_uv;
    uint camera_matrix;
    uint transform_matrix;
    uint texture;
} MaterialAttributesRenderTexture;
zoxc_custom(MaterialAttributesRenderTexture);

zox_tag(RenderTextureScreen);

void define_components_render_textures(ecs* world) {
    zoxd_tag(RenderTextureScreen);
    zoxd(MaterialAttributesRenderTexture);
}
