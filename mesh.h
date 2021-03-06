#ifndef MESH_H
#define MESH_H


#include <vector>
#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 ParaCoord;
};

class Mesh {
    public:
        // mesh data
        std::vector<Vertex>       vertices;
        std::vector<unsigned int> indices;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
        void switchToLine();
        void switchToTriangle();
        void Draw(Shader &shader);
    private:
        //  render data
        unsigned int VAO, VBO, EBO;
        unsigned int mode = 0;
        void setupMesh();
};  

#endif
