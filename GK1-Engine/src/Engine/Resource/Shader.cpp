#include "Engine/Resource/Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : m_shaderProgram(0)
{
}

Shader::~Shader()
{
	glDeleteProgram(m_shaderProgram);
}

std::shared_ptr<Shader> Shader::LoadFromFile(const std::string &vertexPath, const std::string &fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure)
	{
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		return nullptr;
	}

	return LoadFromString(vertexCode, fragmentCode);
}

std::shared_ptr<Shader> Shader::LoadFromString(const std::string &vertexSrc, const std::string &fragmentSrc)
{
	auto shader = std::make_shared<Shader>();

	if (!shader->AddShaderStage(ShaderType::Vertex, vertexSrc) ||
		!shader->AddShaderStage(ShaderType::Fragment, fragmentSrc) ||
		!shader->Link())
	{
		return nullptr;
	}

	return shader;
}

bool Shader::AddShaderStage(ShaderType type, const std::string &source)
{
	GLenum glType;
	switch (type)
	{
		case ShaderType::Vertex:
			glType = GL_VERTEX_SHADER;
			break;
		case ShaderType::Fragment:
			glType = GL_FRAGMENT_SHADER;
			break;
		default:
			return false;
	}

	GLuint shader = glCreateShader(glType);
	const char *sourceCStr = source.c_str();
	glShaderSource(shader, 1, &sourceCStr, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(shader);
		return false;
	}

	if (m_shaderProgram == 0)
	{
		m_shaderProgram = glCreateProgram();
	}

	glAttachShader(m_shaderProgram, shader);
	glDeleteShader(shader);
	return true;
}

bool Shader::Link()
{
	glLinkProgram(m_shaderProgram);

	GLint success;
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(m_shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return false;
	}

	return true;
}

void Shader::Use() const
{
	glUseProgram(m_shaderProgram);
}

GLint Shader::GetUniformLocation(const std::string &name)
{
	auto it = m_uniformLocations.find(name);
	if (it != m_uniformLocations.end())
	{
		return it->second;
	}

	GLint location = glGetUniformLocation(m_shaderProgram, name.c_str());
	m_uniformLocations[name] = location;
	return location;
}

void Shader::SetBool(const std::string &name, bool value)
{
	glUniform1i(GetUniformLocation(name), static_cast<int>(value));
}

void Shader::SetInt(const std::string &name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string &name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string &name, const glm::vec2 &value)
{
	glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value)
{
	glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string &name, const glm::mat3 &value)
{
	glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string &name, const glm::mat4 &value)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}