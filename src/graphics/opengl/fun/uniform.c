static inline void zox_gpu_int(gint index, int value) {
    glUniform1i(index, value);
}

static inline void zox_gpu_float(gint index, float value) {
    glUniform1f(index, value);
}

static inline void zox_gpu_float2(gint index, float2 value) {
    glUniform2f(index, value.x, value.y);
}

static inline void zox_gpu_float3(gint index, float3 value) {
    glUniform3f(index, value.x, value.y, value.z);
}

static inline void zox_gpu_float4(gint index, float4 value) {
    glUniform4f(index, value.x, value.y, value.z, value.w);
}

static inline void zox_gpu_float4x4(gint index, float4x4 value) {
    glUniformMatrix4fv(index, 1, GL_FALSE, (float*) &value);
}

static inline void zox_gpu_float4x4_array(gint index, const float4x4* values, int count) {
    glUniformMatrix4fv(index, count, GL_FALSE, (float*) values);
}

static inline void zox_gpu_float3_array(gint index, const float3* values, gsizei count) {
    glUniform3fv(index, count, (const GLfloat*) values);
}

static inline void zox_gpu_color_rgb(gint index, color_rgb value) {
    float3 valuef3 = color_rgb_to_float3(value);
    zox_gpu_float4(index, (float4) { valuef3.x, valuef3.y, valuef3.z, 1 });
}

static inline void zox_gpu_color(gint index, color value) {
    float4 valuef = color_to_float4(value);
    zox_gpu_float4(index, (float4) { valuef.x, valuef.y, valuef.z, valuef.w });
}
