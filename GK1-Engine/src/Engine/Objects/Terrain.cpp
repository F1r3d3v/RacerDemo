#include "Engine/Objects/Terrain.h"

static constexpr char vertexShaderSource[] = R"(
#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;

void main() {
    vs_out.texCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}
)";

static constexpr char tessControlShaderSource[] = R"(
#version 410 core
layout (vertices = 4) out;

in VS_OUT {
    vec2 texCoords;
} tcs_in[];

out TCS_OUT {
    vec2 texCoords;
} tcs_out[];

uniform float tessLevel;

void main() {
    // Pass through vertex position
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_out[gl_InvocationID].texCoords = tcs_in[gl_InvocationID].texCoords; // Pass through texture coordinates
    
    // Set tessellation levels
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = tessLevel;
        gl_TessLevelOuter[1] = tessLevel;
        gl_TessLevelOuter[2] = tessLevel;
        gl_TessLevelOuter[3] = tessLevel;
        
        gl_TessLevelInner[0] = tessLevel;
        gl_TessLevelInner[1] = tessLevel;
    }
}
)";

static constexpr char tessEvalShaderSource[] = R"(
#version 410 core
layout (quads, fractional_odd_spacing, ccw) in;

in TCS_OUT {
    vec2 texCoords;
} tes_in[];

out TES_OUT {
    vec3 fragPos;
    vec2 texCoords;
	mat3 TBN;
	flat vec3 viewPos;
} tes_out;

uniform mat4 model;
layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

uniform sampler2D heightMap;
uniform float heightScale;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2, vec2 v3, vec2 uv)
{
    vec2 t00 = tes_in[0].texCoords;
    vec2 t01 = tes_in[1].texCoords;
    vec2 t10 = tes_in[2].texCoords;
    vec2 t11 = tes_in[3].texCoords;

    // Bilinear interpolation
    vec2 t0 = (t01 - t00) * uv.x + t00;
    vec2 t1 = (t11 - t10) * uv.x + t10;
    return (t1 - t0) * uv.y + t0;
}

vec4 interpolate4D(vec4 v0, vec4 v1, vec4 v2, vec4 v3, vec2 uv)
{
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    // Bilinear interpolation
    vec4 p0 = (p01 - p00) * uv.x + p00;
    vec4 p1 = (p11 - p10) * uv.x + p10;
    return (p1 - p0) * uv.y + p0;
}

vec3 calcNormal(vec2 texCoord)
{
	const ivec3 off = ivec3(-1,0,1);

	float left = textureOffset(heightMap, texCoord, off.xy).r * heightScale;
	float right = textureOffset(heightMap, texCoord, off.zy).r * heightScale;
	float down = textureOffset(heightMap, texCoord, off.yx).r * heightScale;
	float up = textureOffset(heightMap, texCoord, off.yz).r * heightScale;

    return normalize(vec3(left - right, 2.0, up - down));
}

void main() {
    // Interpolate texture coordinates
    vec2 texCoord = interpolate2D(tes_in[0].texCoords, tes_in[1].texCoords,
                                  tes_in[2].texCoords, tes_in[3].texCoords, gl_TessCoord.xy);
    
    // Interpolate position
    vec4 pos = interpolate4D(gl_in[0].gl_Position, gl_in[1].gl_Position,
                             gl_in[2].gl_Position, gl_in[3].gl_Position, gl_TessCoord.xy);

    // displace point along normal
    pos.y += texture(heightMap, texCoord).r * heightScale;

	// Calculate TBN matrix
    vec3 N = transpose(inverse(mat3(model))) * calcNormal(texCoord);
	vec3 B = normalize(cross(N, vec3(1.0, 0.0, 0.0)));
	vec3 T = normalize(cross(B, N));
    
    // Output
    tes_out.fragPos = vec3(model * pos);
	tes_out.TBN = mat3(T, B, N);
    tes_out.texCoords = texCoord;
	tes_out.viewPos = -vec3(view[3]) * mat3(view);
    
    gl_Position = projection * view * model * pos;
}
)";

static constexpr char fragmentShaderSource[] = R"(
#version 410 core

in TES_OUT {
    vec3 fragPos;
    vec2 texCoords;
    mat3 TBN;
	flat vec3 viewPos;
} fs_in;

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	sampler2D ambientMap0;
	sampler2D diffuseMap0;
	sampler2D specularMap0;
	sampler2D shininessMap0;
	sampler2D normalMap0; 
};
uniform Material material;

struct Light {
    vec4 position;
    vec4 direction;	  // w = 0.0 for point lights and w > 0.0 for spot lights focus exponent
    vec4 color;       // RGB + intensity in w
    vec4 attenuation; // x=constant, y=linear, z=quadratic, w=radius
};

layout (std140) uniform Lights
{
    Light pointLights[4];
    Light spotLights[4];
    ivec4 counts;     // x=numPointLights, y=numSpotLights
	vec4 ambientIntensity;
} lights;

layout (std140) uniform Fog
{
	vec4 color; // intensity in w
	int enabled;
} fog;

float CalcFogFactor(vec3 fragPos)
{
	float gradient = fog.color.w * fog.color.w - 150 * fog.color.w + 180;
	float distance = length(fs_in.viewPos - fragPos);
	
	float fog = exp(-pow(distance/gradient, 4));
	return clamp(fog, 0.0, 1.0);
}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewPos)
{
    float intensity = light.color.w;
    
	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(light.position.xyz - fragPos);
        
    // Calculate attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + 
                        light.attenuation.y * distance +
                        light.attenuation.z * distance * distance);
        
	if (light.direction.w > 0.0) {
		intensity *= pow(max(dot(normalize(-light.direction.xyz), lightDir), 0.0), light.direction.w);
	}
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * texture(material.shininessMap0, fs_in.texCoords).r);
    
    // Combine
    vec3 diffuse = light.color.rgb * diff * material.diffuse * texture(material.diffuseMap0, fs_in.texCoords).rgb;
    vec3 specular = light.color.rgb * spec * material.specular * texture(material.specularMap0, fs_in.texCoords).rgb;
    
    return (diffuse + specular) * attenuation * intensity;
}

void main()
{
	vec3 Normal = texture(material.normalMap0, fs_in.texCoords).rgb;
	Normal = Normal * 2.0 - 1.0;   
	Normal = normalize(fs_in.TBN * Normal); 
    
	// Calculate lighting
	vec3 result = lights.ambientIntensity.rgb *  material.ambient * texture(material.ambientMap0, fs_in.texCoords).rgb;
    for(int i = 0; i < lights.counts.x; i++) {
        result += CalcLight(lights.pointLights[i], Normal, fs_in.fragPos, fs_in.viewPos);
    }
    for(int i = 0; i < lights.counts.y; i++) {
        result += CalcLight(lights.spotLights[i], Normal, fs_in.fragPos, fs_in.viewPos);
    }
    result = clamp(result, 0.0, 1.0);

	if (fog.enabled != 0)
	{
		float fog_factor = CalcFogFactor(fs_in.fragPos);
		result = mix(fog.color.rgb, result, fog_factor);
	}

	FragColor = vec4(result, 1.0);
}
)";

Terrain::Terrain(uint32_t gridSize)
	: m_gridSize(gridSize)
	, m_WorldScale(1.0f)
	, m_tessLevel(16.0f)
	, m_heightScale(128.0f)
	, m_dirty(true)
{
	// Create and bind VAO
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	// Load terrain shaders
	auto shader = std::make_shared<Shader>();
	shader->AddShaderStage(Shader::ShaderType::Vertex, vertexShaderSource);
	shader->AddShaderStage(Shader::ShaderType::TessControl, tessControlShaderSource);
	shader->AddShaderStage(Shader::ShaderType::TessEvaluation, tessEvalShaderSource);
	shader->AddShaderStage(Shader::ShaderType::Fragment, fragmentShaderSource);
	shader->Link();

	m_material = std::make_shared<Material>(shader);
}

Terrain::~Terrain()
{
	Cleanup();
}

void Terrain::GenerateCollisionMesh(uint32_t gridSize, std::vector<glm::vec3> &vertices, std::vector<uint32_t> &indices)
{
	if (!m_heightmap) return;

	float spacing = 1.0f / (gridSize - 1);
	int width = m_heightmap->GetWidth();
	int height = m_heightmap->GetHeight();

	vertices.reserve(gridSize * gridSize);
	indices.reserve((gridSize - 1) * (gridSize - 1) * 4);

	for (uint32_t z = 0; z < gridSize; ++z)
	{
		for (uint32_t x = 0; x < gridSize; ++x)
		{
			float value = m_heightmap->GetPixel<uint16_t>(x * spacing * (width - 1), (1.0f - z * spacing) * (height - 1)).r * 2 * m_heightScale;
			vertices.push_back(glm::vec3(width * (x * spacing - 0.5f) * m_WorldScale, value, height * (z * spacing - 0.5f) * m_WorldScale));
		}
	}

	for (uint32_t z = 0; z < gridSize - 1; ++z)
	{
		for (uint32_t x = 0; x < gridSize - 1; ++x)
		{
			uint32_t topLeft = z * gridSize + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (z + 1) * gridSize + x;
			uint32_t bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);
			indices.push_back(bottomRight);
		}
	}
}

void Terrain::SetupGeometry()
{
	if (!m_heightmap) return;

	// Generate a grid of vertices
	float spacing = 1.0f / (m_gridSize - 1);
	int width = m_heightmap->GetWidth();
	int height = m_heightmap->GetHeight();

	for (uint32_t z = 0; z < m_gridSize; ++z)
	{
		for (uint32_t x = 0; x < m_gridSize; ++x)
		{
			Geometry::Vertex vertex;
			vertex.position = glm::vec3(width * (x * spacing - 0.5f), 0.0f, height * (z * spacing - 0.5f)) * m_WorldScale;
			vertex.texCoords = glm::vec2(x * spacing, 1.0 - z * spacing);
			m_vertices.push_back(vertex);
		}
	}

	// Generate indices for patches (quads)
	for (uint32_t z = 0; z < m_gridSize - 1; ++z)
	{
		for (uint32_t x = 0; x < m_gridSize - 1; ++x)
		{
			uint32_t topLeft = z * m_gridSize + x;
			uint32_t topRight = topLeft + 1;
			uint32_t bottomLeft = (z + 1) * m_gridSize + x;
			uint32_t bottomRight = bottomLeft + 1;

			m_indices.push_back(topLeft);
			m_indices.push_back(bottomLeft);
			m_indices.push_back(topRight);
			m_indices.push_back(bottomRight);
		}
	}

	glBindVertexArray(m_vao);

	// Bind vertices
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Geometry::Vertex),
				 m_vertices.data(), GL_STATIC_DRAW);

	// Bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t),
				 m_indices.data(), GL_STATIC_DRAW);

	// Set vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex),
						  (void *)offsetof(Geometry::Vertex, position));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex),
						  (void *)offsetof(Geometry::Vertex, texCoords));

	glBindVertexArray(0);
}

void Terrain::Draw()
{
	if (!m_heightmap) return;
	if (m_dirty)
	{
		SetupGeometry();
		m_dirty = false;
	}

	m_material->Bind();

	auto shader = m_material->GetShader();
	shader->Use();

	// Set uniforms
	shader->SetMat4("model", GetWorldMatrix());
	shader->SetFloat("tessLevel", m_tessLevel);
	shader->SetFloat("heightScale", m_heightScale);

	// Bind textures
	m_heightmap->Bind(5);
	shader->SetInt("heightMap", 5);

	// Draw patches
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glBindVertexArray(m_vao);
	glDrawElements(GL_PATCHES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Terrain::Cleanup()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}