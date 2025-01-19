#pragma once
#include "Engine/Resource/Resource.h"
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <memory>

class Shader : public Resource {
public:

	enum class ShaderType {
		Vertex,
		Fragment
	};

	Shader();
	~Shader();

	static std::shared_ptr<Shader> LoadFromFile(const std::string &vertexPath, const std::string &fragmentPath);
	static std::shared_ptr<Shader> LoadFromString(const std::string &vertexSrc, const std::string &fragmentSrc);

	bool AddShaderStage(ShaderType type, const std::string &source);
	bool Link();
	void Use() const;

	void SetBool(const std::string &name, bool value);
	void SetInt(const std::string &name, int value);
	void SetFloat(const std::string &name, float value);
	void SetVec2(const std::string &name, const glm::vec2 &value);
	void SetVec3(const std::string &name, const glm::vec3 &value);
	void SetVec4(const std::string &name, const glm::vec4 &value);
	void SetMat3(const std::string &name, const glm::mat3 &value);
	void SetMat4(const std::string &name, const glm::mat4 &value);

	GLuint GetID() const
	{
		return m_shaderProgram;
	}

private:
	GLint GetUniformLocation(const std::string &name);

	GLuint m_shaderProgram;
	std::unordered_map<std::string, GLint> m_uniformLocations;
};