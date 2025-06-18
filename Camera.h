
#define CAMERA_CLASS_H


#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"





class Camera
{
public:
    
    glm::vec3 GetFront() const {
        return glm::normalize(Orientation);
       
       

    }
   
    // Almacena los vectores principales de la cámara
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(-0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 cameraMatrix = glm::mat4(1.0f);

    // Impide que la cámara se mueva bruscamente cuando se hace clic por primera vez
    bool firstClick = true;

    // Almacena el ancho y alto de la ventana
    int width;
    int height;

    // Ajusta la velocidad de la cámara y la sensibilidad al mirar alrededor
    float speed = 3.0f;
    float sensitivity = 100.0f;
    float reduction = 16.34f;

    // Constructor de la cámara para establecer valores iniciales
    Camera(int width, int height, glm::vec3 position);

    // Actualiza la matriz de la cámara en el Vertex Shader
    void updateMatrix(float FOVdeg, float nearPlane, float farPlane);

    // Exporta la matriz de la cámara a un shader
    void Matrix(Shader& shader, const char* uniform);

    // Maneja las entradas del usuario para controlar la cámara
    void Inputs(GLFWwindow* window);
    

   
};