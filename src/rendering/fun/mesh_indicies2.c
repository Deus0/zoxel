void clear_mesh(MeshIndicies *meshIndicies, MeshVertices *meshVertices, MeshColors *meshColors) {
    dispose_MeshIndicies(meshIndicies);
    dispose_MeshVertices(meshVertices);
    dispose_MeshColors(meshColors);
}