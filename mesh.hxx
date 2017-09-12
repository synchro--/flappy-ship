
// A mesh object, loaded from a Wavefront Obj
class Mesh {
private:
  std::vector<Vertex> m_verts; // vettore di vertici
  std::vector<Face> m_faces;   // vettore di facce
  //  std::vector<Edge> m_edges;   // vettore di edge (per ora, non usato)
  // empty constructor. loadMesh must be used instead
  Mesh();

  // Note: the wireframe bool will be retrieved by the "m_env" of whichever
  // class is loading a mesh
  void render(bool wireframe = false, bool gouraud_shading = true);

  // bounding box: minumum and maximum coordinates
  void ComputeBoundingBox();
  void ComputeNormalsPerFace();
  void ComputeNormalsPerVertex();

public:
  // costruttore con caricamento
  /*
  Mesh(char *filename) {
    LoadFromObj(filename);
    ComputeNormalsPerFace();
    ComputeNormalsPerVertex();
    ComputeBoundingBox();
}*/

  // friend function to load the mesh instead of exporting the cons
  friend std::unique_ptr<Mesh> loadMesh(char *mesh_filename);

  // frontend for the render method
  void renderFlat(bool wireframe = false);
  void renderGouraud(bool wireframe = false);

  // center of the axis-aligned bounding box
  inline Point3 Center() { return (bbmin + bbmax) / 2.0; };

  Point3 bbmin, bbmax; // bounding box
};
