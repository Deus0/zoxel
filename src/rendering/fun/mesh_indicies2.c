void clear_mesh(MeshIndicies *meshIndicies, MeshVertices *meshVertices, MeshColorRGBs *meshColorRGBs) {
    dispose_MeshIndicies(meshIndicies);
    dispose_MeshVertices(meshVertices);
    dispose_MeshColorRGBs(meshColorRGBs);
}