#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader
{
public:
	Shader();
	Shader(const char* vertexPath, const char* fragmentPath);

	unsigned int ID;  // program ID

	void use();  // use/activate shader
	void set1b(const std::string& name, bool value) const;
	void set1i(const std::string& name, int value) const;
	void set1f(const std::string& name, float value) const;
	void set2f(const std::string& name, float value1, float value2) const;
	void set2f(const std::string& name, glm::vec2) const;
	void set3f(const std::string& name, float value1, float value2,
		float value3) const;
	void set3f(const std::string& name, glm::vec3 vector) const;
	void set4f(const std::string& name, float value1, float value2, float value3,
		float value4) const;
	void set1mat4f(const std::string& name, glm::mat4 matrix);
};

#endif