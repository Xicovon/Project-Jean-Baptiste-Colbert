#pragma once

class BufferWrapper {
private:
	GLuint VAO, indices_VBO, vertices_VBO, color_VBO;
	unsigned int indices_size;

public:
	BufferWrapper(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices, std::vector<glm::vec4> colors) {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &vertices_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &indices_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_VBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &color_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, color_VBO);

		glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), &colors.front(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

		indices_size = indices.size();
	}

	GLuint GetVAO() { return VAO; }
	GLuint GetVertexVBO() { return vertices_VBO; }
	GLuint GetIndexVBO() { return indices_VBO; }
	GLuint GetColorVBO() { return color_VBO; }
	unsigned int GetIndicesSize() { return indices_size; }
};