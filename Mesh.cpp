#include "Mesh.h"




Mesh::Mesh
(
	std::vector <Vertex>& vertices,
	std::vector <GLuint>& indices,
	std::vector <Texture>& textures,
	unsigned int instancing,
	std::vector <glm::mat4> instanceMatrix
)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::instancing = instancing;

	VAO.Bind();
	// Generates Vertex Buffer Object and links it to vertices
	VBO instanceVBO(instanceMatrix);
	VBO VBO(vertices);
	// Generates Element Buffer Object and links it to indices
	EBO EBO(indices);
	// Links VBO attributes such as coordinates and colors to VAO
	VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	if (instancing != 1)
	{
		instanceVBO.Bind();
		// Can't link to a mat4 so you need to link four vec4s
		VAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		VAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		VAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		VAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		// Makes it so the transform is only switched when drawing the next instance
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
	}
	// Unbind all to prevent accidentally modifying them
	VAO.Unbind();
	VBO.Unbind();
	instanceVBO.Unbind();
	EBO.Unbind();
}


void Mesh::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	// Bind shader to be able to access uniforms
	shader.Activate();
	VAO.Bind();

	unsigned int numDiffuse = 0;
	unsigned int numEmissive = 0;

	for (int i = 0; i < textures.size(); i++)
{
    std::string name;
    if (textures[i].type == "diffuse")
        name = "diffuse[" + std::to_string(numDiffuse++) + "]";
    else if (textures[i].type == "emissive")
        name = "emissive[" + std::to_string(numEmissive++) + "]";

    textures[i].texUnit(shader, name.c_str(), i);
    textures[i].Bind();
}


	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	// Check if instance drawing should be performed
	
	
		// Initialize matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		// Transform the matrices to their correct form
		trans = glm::translate(trans, translation);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);

		glm::quat weaponRotation = glm::quatLookAt(camera.Orientation, camera.Up);
		rot = glm::mat4_cast(weaponRotation);

		// Aplica una rotaci�n adicional si es necesario
		rot = glm::rotate(rot, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // Rota adicionalmente

		/*trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));*/
		/*sca = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f));*/

		// Push the matrices to the vertex shader
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	
	
}