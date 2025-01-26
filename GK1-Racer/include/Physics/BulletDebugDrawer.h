#pragma once
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>
#include <glad/gl.h>
#include <memory>
#include <Engine/Resource/Shader.h>

class BulletDebugDrawer : public btIDebugDraw {
public:
	BulletDebugDrawer() : m(0) {
		constexpr char vertexShaderSource[] = R"(	
			#version 330 core
			layout (location = 0) in vec3 aPos;

			layout (std140) uniform Matrices
			{
				mat4 view;
				mat4 projection;
			};

			void main()
			{
				gl_Position = projection * view * vec4(aPos, 1.0);
			}
		)";

		constexpr char fragmentShaderSource[] = R"(
			#version 330 core

			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(1.0, 1.0, 0.0, 1.0);
			}
		)";

		m_shader = Shader::LoadFromString(vertexShaderSource, fragmentShaderSource);
		m_shader->BindUBO("Matrices", 0);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	~BulletDebugDrawer() {
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
	}

	struct LineValues
	{
		btVector3 p1;
		btVector3 p2;

	};

	virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
	{
		// Draws a simple line of pixels between points but stores them for later draw
		LineValues Line;
		Line.p1 = from;
		Line.p2 = to;

		TheLines.push_back(Line);
	}
	virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
	virtual void reportErrorWarning(const char *) {}
	virtual void draw3dText(const btVector3 &, const char *) {}
	virtual void setDebugMode(int p) {
		m = p;
	}
	int getDebugMode(void) const { return DBG_DrawWireframe; }
	int m;


	void Draw()
	{
		m_shader->Use();
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, TheLines.size() * sizeof(LineValues), TheLines.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(m_vao);
		glDrawArrays(GL_LINES, 0, (GLsizei)(2 * TheLines.size()));

		TheLines.clear();
	}


private:
	GLuint m_vao, m_vbo;
	std::shared_ptr<Shader> m_shader;
	std::vector<LineValues> TheLines;
};