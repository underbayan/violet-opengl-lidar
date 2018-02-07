#ifndef MeshUtilities_h
#define MeshUtilities_h

#include <string>
#include <vector>
#include <glm/glm.hpp>

// A mesh will be represented by a struct. For now, material information and elements/groups are not retrieved from the .obj.
struct mesh_t {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> binormals;
    std::vector<glm::vec2> texcoords;
    std::vector<unsigned int> indices;

    void clear() {
        indices.clear();
        positions.clear();
        normals.clear();
        texcoords.clear();
    }
};

// Three load modes: load the vertices without any connectivity (Points), 
// 					 load them with all vertices duplicated for each face (Expanded),
//					 load them after duplicating only the ones that are shared between faces with multiple attributes (Indexed).
enum LoadMode {
    Expanded, Points, Indexed
};

/// Load an obj file from disk into the mesh structure.
void loadObj(const std::string &filename, mesh_t &mesh, LoadMode mode);

/// Center the mesh and scale it to fit in the [-1,1] box.
void centerAndUnitMesh(mesh_t &mesh);

/// Compute the tangents and binormal vectors for each vertex.
void computeTangentsAndBinormals(mesh_t &mesh);

#endif

