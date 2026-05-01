byte does_support_geometry_shaders() {
#ifndef zox_disable_geometry_shaders
    gint maxGeometryOutputVertices;
    glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &maxGeometryOutputVertices);
    return (maxGeometryOutputVertices > 0);
#else
    return 0;
#endif
}

void check_geometry() {
    if (does_support_geometry_shaders()) {
        zox_log(" + geometry shader supported\n")
    } else {
        zox_log(" ! geometry shader unsupported\n")
    }
}
