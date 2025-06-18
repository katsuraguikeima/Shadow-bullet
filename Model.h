#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include "Libraries/LIBS/nlohmann/json.hpp"  // CORRECTO INCLUDE
#include "Mesh.h"
#include "Atlas.h"


using json = nlohmann::json;

class Model
{
public:
    // Constructor para cargar el modelo y las matrices de instancias
    Model(const char* file, unsigned int instancing = 1, std::vector<glm::mat4> instanceMatrix = {});
    /* agregado para el proyectil*/
    // Método para dibujar instancias del modelo para el proyectil 
    void DrawInstanced(Shader& shader, const std::vector<glm::mat4>& instanceMatrices)
    {
        if (instanceMatrices.empty()) return;

        // Crear o actualizar el VBO de instancias
        if (instanceVBO == 0)
            glGenBuffers(1, &instanceVBO);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceMatrices.size() * sizeof(glm::mat4), instanceMatrices.data(), GL_DYNAMIC_DRAW);

        for (auto& mesh : meshes) {
            glBindVertexArray(mesh.VAO.ID);  


            // Atributos para la matriz model (4 vec4 = 4 atributos)
            std::size_t vec4Size = sizeof(glm::vec4);
            for (int i = 0; i < 4; i++) {
                glEnableVertexAttribArray(5 + i);
                glVertexAttribPointer(5 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
                glVertexAttribDivisor(5 + i, 1); // una vez por instancia
            }

            // Dibujar instanciado
            glDrawElementsInstanced(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0, instanceMatrices.size());

            glBindVertexArray(0);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }


    // Método para dibujar una instancia del modelo con transformaciones adicionales
    void Draw(Shader& shader, Camera& camera, glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

private:
    const char* file;
    std::vector<unsigned char> data;
    json JSON;
    unsigned int instancing;
    /* para el proyectil*/
    unsigned int instanceVBO = 0;  // VBO para matrices de instancia

    // NUEVO: Puntero a Atlas
    Atlas* atlas = nullptr;

    std::vector<Mesh> meshes;
    std::vector<glm::vec3> translationsMeshes;
    std::vector<glm::quat> rotationsMeshes;
    std::vector<glm::vec3> scalesMeshes;
    std::vector<glm::mat4> matricesMeshes;
    std::vector<glm::mat4> instanceMatrix;

    std::vector<std::string> loadedTexName;
    std::vector<Texture> loadedTex;

    void loadMesh(unsigned int indMesh);
    void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));
    std::vector<unsigned char> getData();
    std::vector<float> getFloats(json accessor);
    std::vector<GLuint> getIndices(json accessor);
    std::vector<Texture> getTextures();

    std::vector<Vertex> assembleVertices(
        std::vector<glm::vec3> positions,
        std::vector<glm::vec3> normals,
        std::vector<glm::vec2> texUVs
    );

    std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
    std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
    std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);

    unsigned int VAO;  // Asegúrate de que este miembro esté definido si usas VAO
    std::vector<GLuint> indices;  // Asegúrate de tener los índices cargados
};

#endif
