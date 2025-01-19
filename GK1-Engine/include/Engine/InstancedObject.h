#pragma once
#include "Engine/Objects/GraphicsObject.h"
#include <glad/gl.h>
#include <cstdint>

class InstancedObject : public GraphicsObject
{
public:
	InstancedObject(uint32_t InstanceCount, uint32_t ShaderProgram);
	~InstancedObject();

protected:
	uint32_t m_InstanceCount;
	GLuint m_ShaderProgram;
	GLuint m_ParticleVAO;
	GLuint m_ParticleVBO;
	GLuint m_ParticleEBO;
	GLuint m_InstanceVBO;
};

