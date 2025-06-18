#include"Camera.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // Calculate the view matrix using the position and orientation of the camera
    view = glm::lookAt(Position, Position + Orientation, Up);

    // Create a perspective projection matrix with the given FOV and aspect ratio
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);


    // Update the camera matrix by multiplying projection and view matrices
    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    // Exports camera matrix
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

glm::vec3 lerp(const glm::vec3& start, const glm::vec3& end, float alpha)
{
    return start + alpha * (end - start);
}




void Camera::Inputs(GLFWwindow* window)
{
    // Límite de la posición de la cámara para evitar que se salga de los límites
    Position.x = glm::clamp(Position.x, -200.0f, 200.0f);
    Position.z = glm::clamp(Position.z, -200.0f, 200.0f);

    glm::vec3 targetPosition = Position;

    // Movimiento suave con las teclas WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        targetPosition += speed * Orientation * glm::vec3(1.0f, 0.0f, 1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        targetPosition += speed * -glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        targetPosition += speed * -Orientation * glm::vec3(1.0f, 0.0f, 1.0f);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        targetPosition += speed * glm::normalize(glm::cross(Orientation, Up));
    }

    // Movimiento suave de la cámara
    Position = lerp(Position, targetPosition, 0.1f);  // Suaviza el movimiento



    // Tecla Escape para cerrar la ventana
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);

    }

    // Primer click para ocultar el cursor
    if (firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetCursorPos(window, (width / 2), (height / 2));
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        firstClick = false;
    }

    // Control del mouse
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !firstClick)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normaliza el movimiento del mouse y aplica sensibilidad
        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        // Rotación suave en el eje vertical (pitch)
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        // Limitar el ángulo vertical para evitar girar completamente hacia arriba o abajo
        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        // Rotación en el eje horizontal (yaw)
        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

        // Asegurarse de que la orientación está normalizada
        Orientation = glm::normalize(Orientation);

        // Restablecer la posición del cursor al centro de la pantalla para evitar parpadeos
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
}
