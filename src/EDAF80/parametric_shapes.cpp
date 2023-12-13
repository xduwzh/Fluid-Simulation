#include "parametric_shapes.hpp"
#include "core/Log.h"

#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <assimp/scene.h>

bonobo::mesh_data
parametric_shapes::createQuad(float const width, float const height,
                              unsigned int const horizontal_split_count,
                              unsigned int const vertical_split_count)
{

	auto const horizontal_slice_edges_count = horizontal_split_count + 1u;
	auto const vertical_slice_edges_count = vertical_split_count + 1u;
	auto const horizontal_slice_vertices_count = horizontal_slice_edges_count + 1u;
	auto const vertical_slice_vertices_count = vertical_slice_edges_count + 1u;
	auto const vertices_nb = horizontal_slice_vertices_count * vertical_slice_vertices_count;

	float const d_horizontal = width / (static_cast<float>(horizontal_slice_edges_count));
	float const d_vertical = height / (static_cast<float>(vertical_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float hor = 0.0f;
	float ver = 0.0f;
	auto vertices = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	for (unsigned int i = 0u; i < horizontal_slice_vertices_count; ++i) {
		ver = 0.0f;
		for (unsigned int j = 0u; j < vertical_slice_vertices_count; ++j) {

			vertices[index] = glm::vec3(hor ,0.0f, ver);
			texcoords[index] = glm::vec3(static_cast<float>(i) / (static_cast<float>(horizontal_slice_vertices_count)),
				static_cast<float>(j) / (static_cast<float>(vertical_slice_vertices_count)),
				0.0f);
			++index;
			ver += d_vertical;
		}
		hor += d_horizontal;
	}

	//auto const vertices = std::array<glm::vec3, 4>{
	//	glm::vec3(0.0f,  0.0f,   0.0f),
	//	glm::vec3(width, 0.0f,   0.0f),
	//	glm::vec3(width, height, 0.0f),
	//	glm::vec3(0.0f,  height, 0.0f)
	//};
	auto index_sets = std::vector<glm::uvec3>(2u * horizontal_slice_vertices_count * vertical_slice_vertices_count);

	//auto const index_sets = std::array<glm::uvec3, 2>{
	//	glm::uvec3(0u, 1u, 2u),
	//	glm::uvec3(0u, 2u, 3u)
	//};
	index = 0u;
	for (unsigned int i = 0u; i < vertical_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < horizontal_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(horizontal_slice_vertices_count * (i + 0u) + (j + 0u),
				horizontal_slice_vertices_count * (i + 0u) + (j + 1u),
				horizontal_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(horizontal_slice_vertices_count * (i + 0u) + (j + 0u),
				horizontal_slice_vertices_count * (i + 1u) + (j + 1u),
				horizontal_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;

		}
	}

	bonobo::mesh_data data;

	//if (horizontal_split_count > 0u || vertical_split_count > 0u)
	//{
	//	LogError("parametric_shapes::createQuad() does not support tesselation.");
	//	return data;
	//}

	//
	// NOTE:
	//
	// Only the values preceeded by a `\todo` tag should be changed, the
	// other ones are correct!
	//

	// Create a Vertex Array Object: it will remember where we stored the
	// data on the GPU, and  which part corresponds to the vertices, which
	// one for the normals, etc.
	//
	// The following function will create new Vertex Arrays, and pass their
	// name in the given array (second argument). Since we only need one,
	// pass a pointer to `data.vao`.
	//glGenVertexArrays(1, /*! \todo fill me */nullptr);
	glGenVertexArrays(1, &data.vao);

	// To be able to store information, the Vertex Array has to be bound
	// first.
	//glBindVertexArray(/*! \todo bind the previously generated Vertex Array */0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const texcoords_offset = vertices_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + texcoords_size);

	// To store the data, we need to allocate buffers on the GPU. Let's
	// allocate a first one for the vertices.
	//
	// The following function's syntax is similar to `glGenVertexArray()`:
	// it will create multiple OpenGL objects, in this case buffers, and
	// return their names in an array. Have the buffer's name stored into
	// `data.bo`.
	//glGenBuffers(1, /*! \todo fill me */nullptr);
	glGenBuffers(1, &data.bo);

	// Similar to the Vertex Array, we need to bind it first before storing
	// anything in it. The data stored in it can be interpreted in
	// different ways. Here, we will say that it is just a simple 1D-array
	// and therefore bind the buffer to the corresponding target.
	//glBindBuffer(GL_ARRAY_BUFFER, /*! \todo bind the previously generated Buffer */0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);

	//glBufferData(GL_ARRAY_BUFFER, /*! \todo how many bytes should the buffer contain? */0u,
	//             /* where is the data stored on the CPU? */vertices.data(),
	//             /* inform OpenGL that the data is modified once, but used often */GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, bo_size, vertices.data(), /* inform OpenGL that the data is modified once, but used often */GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	//glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	//glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	// Vertices have been just stored into a buffer, but we still need to
	// tell Vertex Array where to find them, and how to interpret the data
	// within that buffer.
	//
	// You will see shaders in more detail in lab 3, but for now they are
	// just pieces of code running on the GPU and responsible for moving
	// all the vertices to clip space, and assigning a colour to each pixel
	// covered by geometry.
	// Those shaders have inputs, some of them are the data we just stored
	// in a buffer object. We need to tell the Vertex Array which inputs
	// are enabled, and this is done by the following line of code, which
	// enables the input for vertices:
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));

	// Once an input is enabled, we need to explain where the data comes
	// from, and how it interpret it. When calling the following function,
	// the Vertex Array will automatically use the current buffer bound to
	// GL_ARRAY_BUFFER as its source for the data. How to interpret it is
	// specified below:
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices),
	                      /*! \todo how many components do our vertices have? */3,
	                      /* what is the type of each component? */GL_FLOAT,
	                      /* should it automatically normalise the values stored */GL_FALSE,
	                      /* once all components of a vertex have been read, how far away (in bytes) is the next vertex? */0,
	                      /* how far away (in bytes) from the start of the buffer is the first vertex? */reinterpret_cast<GLvoid const*>(0x0));
	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	data.indices_nb = /*! \todo how many indices do we have? */static_cast<GLsizei>(index_sets.size() * 3u);
	// Now, let's allocate a second one for the indices.
	//
	// Have the buffer's name stored into `data.ibo`.
	glGenBuffers(1, &data.ibo);

	// We still want a 1D-array, but this time it should be a 1D-array of
	// elements, aka. indices!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, /*! \todo how many bytes should the buffer contain? */ static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)),
	             /* where is the data stored on the CPU? */index_sets.data(),
	             /* inform OpenGL that the data is modified once, but used often */GL_STATIC_DRAW);


	// All the data has been recorded, we can unbind them.
	glBindVertexArray(0u);
	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}

bonobo::mesh_data
parametric_shapes::createSphere(float const radius,
                                unsigned int const longitude_split_count,
                                unsigned int const latitude_split_count)
{
	//! \todo Implement this function
	auto const longitude_slice_edges_count = longitude_split_count + 1u;
	auto const latitude_slice_edges_count = latitude_split_count + 1u;
	auto const longitude_slice_vertices_count = longitude_slice_edges_count + 1u;
	auto const latitude_slice_vertices_count = latitude_slice_edges_count + 1u;
	auto const vertices_nb = longitude_slice_vertices_count * latitude_slice_vertices_count;

	auto vertices = std::vector<glm::vec3>(vertices_nb);
	auto normals = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	float const d_theta = glm::two_pi<float>() / (static_cast<float>(longitude_slice_edges_count));
	float const d_phi = glm::pi<float>() / (static_cast<float>(latitude_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	float phi = 0.0f;
	for (unsigned int i = 0u; i < latitude_slice_vertices_count; ++i) {
		float const cos_phi = std::cos(phi);
		float const sin_phi = std::sin(phi);
		theta = 0.0f;
		for (unsigned int j = 0u; j < longitude_slice_vertices_count; ++j) {
			

			float const cos_theta = std::cos(theta);
			float const sin_theta = std::sin(theta);
			// vertex
			vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
				- radius * cos_phi,
				radius * cos_theta * sin_phi);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(latitude_slice_vertices_count)),
				static_cast<float>(i) / (static_cast<float>(longitude_slice_vertices_count)),
				0.0f);

			// tangent
			auto const t = glm::vec3(radius * cos_theta, 0, -radius * sin_theta);
			//auto const t = glm::vec3(radius * cos_theta * sin_phi, 0, -radius * sin_theta * sin_phi);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(radius * sin_theta * cos_phi, radius * sin_phi, radius * cos_theta * cos_phi);
			//auto const b = glm::vec3(radius * sin_theta * cos_phi, radius * sin_phi, radius * cos_theta * cos_phi);
			binormals[index] = b;

			// normal
			auto const n = glm::cross(t, b);
			//auto const n = glm::cross(t, b));
			normals[index] = n;
			++index;
			theta += d_theta;
		}
		
		phi += d_phi;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * longitude_slice_vertices_count * latitude_slice_vertices_count);

	// generate indices iterativelyyu
	index = 0u;
	for (unsigned int i = 0u; i <= latitude_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j <= longitude_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(longitude_slice_edges_count * (i + 0u) + (j + 0u),
				longitude_slice_edges_count * (i + 0u) + (j + 1u),
				longitude_slice_edges_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(longitude_slice_edges_count * (i + 0u) + (j + 0u),
				longitude_slice_edges_count * (i + 1u) + (j + 1u),
				longitude_slice_edges_count * (i + 1u) + (j + 0u));
			++index;

		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
		+ normals_size
		+ texcoords_size
		+ tangents_size
		+ binormals_size
		);
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;

	//return bonobo::mesh_data();
}

bonobo::mesh_data
parametric_shapes::createTorus(float const major_radius,
                               float const minor_radius,
                               unsigned int const major_split_count,
                               unsigned int const minor_split_count)
{
	//! \todo (Optional) Implement this function
	//Assimp::Importer importer;
	//const aiScene* scene = importer.ReadFile("F:/desktop/CourseFile/ComputerGraphics/res/models/Scout_Boat.fbx", aiProcess_Triangulate | aiProcess_FlipUVs);
	//if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	//	std::cout << "Error loading FBX file: " << importer.GetErrorString() << std::endl;
	//	return;
	//}
	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec3> normals;
	//std::vector<glm::vec2> texcoords;
	//std::vector<glm::vec3> index_sets;



	//for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
	//	aiMesh* mesh = scene->mMeshes[meshIndex];

	//	for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
	//		aiVector3D vertex = mesh->mVertices[vertexIndex];
	//		glm::vec3 glmVertex(vertex.x, vertex.y, vertex.z);
	//		vertices.push_back(glmVertex);
	//	}
	//	if (mesh->HasNormals()) {
	//		for (unsigned int normalIndex = 0; normalIndex < mesh->mNumVertices; normalIndex++) {
	//			aiVector3D normal = mesh->mNormals[normalIndex];
	//			glm::vec3 glmNormal(normal.x, normal.y, normal.z);
	//			normals.push_back(glmNormal);
	//		}
	//	}
	//	if (mesh->HasTextureCoords(0)) {
	//		for (unsigned int texCoordIndex = 0; texCoordIndex < mesh->mNumVertices; texCoordIndex++) {
	//			aiVector3D texCoord = mesh->mTextureCoords[0][texCoordIndex];
	//			glm::vec2 glmTexCoord(texCoord.x, texCoord.y);
	//			texcoords.push_back(glmTexCoord);
	//		}
	//	}
	//	if (mesh->HasFaces()) {
	//		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
	//			aiFace face = mesh->mFaces[faceIndex];
	//			glm::vec3 verticeIndex;
	//			verticeIndex.x = face.mIndices[0];
	//			verticeIndex.y = face.mIndices[1];
	//			verticeIndex.z = face.mIndices[2];
	//			index_sets.push_back(verticeIndex);
	//		}
	//	}
	//}
	//std::cout << "Vertices number: " << vertices.size() << std::endl;

	//bonobo::mesh_data data;
	//glGenVertexArrays(1, &data.vao);
	//assert(data.vao != 0u);
	//glBindVertexArray(data.vao);
	//auto const vertices_offset = 0u;
	//auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	//auto const normals_offset = vertices_size;
	//auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	//auto const texcoords_offset = normals_offset + normals_size;
	//auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	//auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	//	+ normals_size
	//	+ texcoords_size
	//	);
	//glGenBuffers(1, &data.bo);
	//assert(data.bo != 0u);
	//glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	//glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	//glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	//glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	//glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	//glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	//glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	//glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	//glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	//glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	//glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	//glBindBuffer(GL_ARRAY_BUFFER, 0u);
	//data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	//glGenBuffers(1, &data.ibo);
	//assert(data.ibo != 0u);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	//glBindVertexArray(0u);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	//bonobo::mesh_data test_ship = data;
	//if (test_ship.vao == 0u) {
	//	LogError("Failed to retrieve the mesh for the ship");
	//	return;
	//}
	//Node ship;
	//ship.set_geometry(test_ship);
	//ship.set_program(&fallback_shader, set_uniforms);
	return bonobo::mesh_data();
}

bonobo::mesh_data
parametric_shapes::createCircleRing(float const radius,
                                    float const spread_length,
                                    unsigned int const circle_split_count,
                                    unsigned int const spread_split_count)
{
	auto const circle_slice_edges_count = circle_split_count + 1u;
	auto const spread_slice_edges_count = spread_split_count + 1u;
	auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
	auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
	auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

	auto vertices  = std::vector<glm::vec3>(vertices_nb);
	auto normals   = std::vector<glm::vec3>(vertices_nb);
	auto texcoords = std::vector<glm::vec3>(vertices_nb);
	auto tangents  = std::vector<glm::vec3>(vertices_nb);
	auto binormals = std::vector<glm::vec3>(vertices_nb);

	float const spread_start = radius - 0.5f * spread_length;
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
	float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float distance_to_centre = spread_start;
		for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
			// vertex
			vertices[index] = glm::vec3(distance_to_centre * cos_theta,
			                            distance_to_centre * sin_theta,
			                            0.0f);

			// texture coordinates
			texcoords[index] = glm::vec3(static_cast<float>(j) / (static_cast<float>(spread_slice_vertices_count)),
			                             static_cast<float>(i) / (static_cast<float>(circle_slice_vertices_count)),
			                             0.0f);

			// tangent
			auto const t = glm::vec3(cos_theta, sin_theta, 0.0f);
			tangents[index] = t;

			// binormal
			auto const b = glm::vec3(-sin_theta, cos_theta, 0.0f);
			binormals[index] = b;

			// normal
			auto const n = glm::cross(t, b);
			normals[index] = n;

			distance_to_centre += d_spread;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < circle_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < spread_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 0u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 1u),
			                               spread_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const normals_offset = vertices_size;
	auto const normals_size = static_cast<GLsizeiptr>(normals.size() * sizeof(glm::vec3));
	auto const texcoords_offset = normals_offset + normals_size;
	auto const texcoords_size = static_cast<GLsizeiptr>(texcoords.size() * sizeof(glm::vec3));
	auto const tangents_offset = texcoords_offset + texcoords_size;
	auto const tangents_size = static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3));
	auto const binormals_offset = tangents_offset + tangents_size;
	auto const binormals_size = static_cast<GLsizeiptr>(binormals.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size
	                                            +normals_size
	                                            +texcoords_size
	                                            +tangents_size
	                                            +binormals_size
	                                            );
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, normals_offset, normals_size, static_cast<GLvoid const*>(normals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::normals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::normals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(normals_offset));

	glBufferSubData(GL_ARRAY_BUFFER, texcoords_offset, texcoords_size, static_cast<GLvoid const*>(texcoords.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::texcoords));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::texcoords), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(texcoords_offset));

	glBufferSubData(GL_ARRAY_BUFFER, tangents_offset, tangents_size, static_cast<GLvoid const*>(tangents.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::tangents));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::tangents), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(tangents_offset));

	glBufferSubData(GL_ARRAY_BUFFER, binormals_offset, binormals_size, static_cast<GLvoid const*>(binormals.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::binormals));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::binormals), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(binormals_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}
bonobo::mesh_data
parametric_shapes::createBriefCircleRingBatch2(float const radius,
	float const spread_length,
	unsigned int const circle_split_count,
	unsigned int const spread_split_count, int particleNum, std::vector<glm::vec2>& positions, std::vector<glm::vec2>& velocities)
{
	auto const circle_slice_edges_count = circle_split_count + 1u;
	auto const spread_slice_edges_count = spread_split_count + 1u;
	auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
	auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
	auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

	auto vertices = std::vector<glm::vec3>(vertices_nb);

	float const spread_start = radius - 0.5f * spread_length;
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
	float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float distance_to_centre = spread_start;
		for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
			// vertex
			vertices[index] = glm::vec3(distance_to_centre * cos_theta,
				distance_to_centre * sin_theta,
				0.0f);
			distance_to_centre += d_spread;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < circle_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < spread_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
				spread_slice_vertices_count * (i + 0u) + (j + 1u),
				spread_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
				spread_slice_vertices_count * (i + 1u) + (j + 1u),
				spread_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const positions_offset = vertices_size;
	auto const positions_size = static_cast<GLsizeiptr>(positions.size() * sizeof(glm::vec2));
	auto const velocities_offset = positions_offset + positions_size;
	auto const velocities_size = static_cast<GLsizeiptr>(velocities.size() * sizeof(glm::vec2));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + positions_size +velocities_size);
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, positions_offset, positions_size, static_cast<GLvoid const*>(positions.data()));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<GLvoid const*>(positions_offset));
	glVertexAttribDivisor(1, 1);
	glBufferSubData(GL_ARRAY_BUFFER, velocities_offset, velocities_size, static_cast<GLvoid const*>(velocities.data()));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<GLvoid const*>(velocities_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	data.vertices_nb = vertices_nb;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}
bonobo::mesh_data
	parametric_shapes::createBriefCircleRingBatch3(float const radius,
		float const spread_length,
		unsigned int const circle_split_count,
		unsigned int const spread_split_count, int particleNum, std::vector<glm::vec2>* positions, std::vector<glm::vec2>* velocities)
{
	auto const circle_slice_edges_count = circle_split_count + 1u;
	auto const spread_slice_edges_count = spread_split_count + 1u;
	auto const circle_slice_vertices_count = circle_slice_edges_count + 1u;
	auto const spread_slice_vertices_count = spread_slice_edges_count + 1u;
	auto const vertices_nb = circle_slice_vertices_count * spread_slice_vertices_count;

	auto vertices = std::vector<glm::vec3>(vertices_nb);

	float const spread_start = radius - 0.5f * spread_length;
	float const d_theta = glm::two_pi<float>() / (static_cast<float>(circle_slice_edges_count));
	float const d_spread = spread_length / (static_cast<float>(spread_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	for (unsigned int i = 0u; i < circle_slice_vertices_count; ++i) {
		float const cos_theta = std::cos(theta);
		float const sin_theta = std::sin(theta);

		float distance_to_centre = spread_start;
		for (unsigned int j = 0u; j < spread_slice_vertices_count; ++j) {
			// vertex
			vertices[index] = glm::vec3(distance_to_centre * cos_theta,
				distance_to_centre * sin_theta,
				0.0f);
			distance_to_centre += d_spread;
			++index;
		}

		theta += d_theta;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * circle_slice_edges_count * spread_slice_edges_count);

	// generate indices iteratively
	index = 0u;
	for (unsigned int i = 0u; i < circle_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j < spread_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
				spread_slice_vertices_count * (i + 0u) + (j + 1u),
				spread_slice_vertices_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(spread_slice_vertices_count * (i + 0u) + (j + 0u),
				spread_slice_vertices_count * (i + 1u) + (j + 1u),
				spread_slice_vertices_count * (i + 1u) + (j + 0u));
			++index;
		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const positions_offset = vertices_size;
	auto const positions_size = static_cast<GLsizeiptr>(positions->size() * sizeof(glm::vec2));
	auto const velocities_offset = positions_offset + positions_size;
	auto const velocities_size = static_cast<GLsizeiptr>(velocities->size() * sizeof(glm::vec2));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + positions_size + velocities_size);
	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, positions_offset, positions_size, static_cast<GLvoid const*>(positions->data()));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<GLvoid const*>(positions_offset));

	glBufferSubData(GL_ARRAY_BUFFER, velocities_offset, velocities_size, static_cast<GLvoid const*>(velocities->data()));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<GLvoid const*>(velocities_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	data.vertices_nb = vertices_nb;
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;
}
bonobo::mesh_data
	parametric_shapes::createSphereBatch(float const radius,
		unsigned int const longitude_split_count,
		unsigned int const latitude_split_count, int particleNum, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& velocities)
{
	//! \todo Implement this function
	auto const longitude_slice_edges_count = longitude_split_count + 1u;
	auto const latitude_slice_edges_count = latitude_split_count + 1u;
	auto const longitude_slice_vertices_count = longitude_slice_edges_count + 1u;
	auto const latitude_slice_vertices_count = latitude_slice_edges_count + 1u;
	auto const vertices_nb = longitude_slice_vertices_count * latitude_slice_vertices_count;

	auto vertices = std::vector<glm::vec3>(vertices_nb);

	float const d_theta = glm::two_pi<float>() / (static_cast<float>(longitude_slice_edges_count));
	float const d_phi = glm::pi<float>() / (static_cast<float>(latitude_slice_edges_count));

	// generate vertices iteratively
	size_t index = 0u;
	float theta = 0.0f;
	float phi = 0.0f;
	for (unsigned int i = 0u; i < latitude_slice_vertices_count; ++i) {
		float const cos_phi = std::cos(phi);
		float const sin_phi = std::sin(phi);
		theta = 0.0f;
		for (unsigned int j = 0u; j < longitude_slice_vertices_count; ++j) {


			float const cos_theta = std::cos(theta);
			float const sin_theta = std::sin(theta);
			// vertex
			vertices[index] = glm::vec3(radius * sin_theta * sin_phi,
				-radius * cos_phi,
				radius * cos_theta * sin_phi);
			++index;
			theta += d_theta;
		}

		phi += d_phi;
	}

	// create index array
	auto index_sets = std::vector<glm::uvec3>(2u * longitude_slice_vertices_count * latitude_slice_vertices_count);

	// generate indices iterativelyyu
	index = 0u;
	for (unsigned int i = 0u; i <= latitude_slice_edges_count; ++i)
	{
		for (unsigned int j = 0u; j <= longitude_slice_edges_count; ++j)
		{
			index_sets[index] = glm::uvec3(longitude_slice_edges_count * (i + 0u) + (j + 0u),
				longitude_slice_edges_count * (i + 0u) + (j + 1u),
				longitude_slice_edges_count * (i + 1u) + (j + 1u));
			++index;

			index_sets[index] = glm::uvec3(longitude_slice_edges_count * (i + 0u) + (j + 0u),
				longitude_slice_edges_count * (i + 1u) + (j + 1u),
				longitude_slice_edges_count * (i + 1u) + (j + 0u));
			++index;

		}
	}

	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const positions_offset = vertices_size;
	auto const positions_size = static_cast<GLsizeiptr>(positions.size() * sizeof(glm::vec3));
	auto const velocities_offset = positions_offset + positions_size;
	auto const velocities_size = static_cast<GLsizeiptr>(velocities.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + positions_size + velocities_size);

	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, positions_offset, positions_size, static_cast<GLvoid const*>(positions.data()));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(positions_offset));

	glBufferSubData(GL_ARRAY_BUFFER, velocities_offset, velocities_size, static_cast<GLvoid const*>(velocities.data()));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(velocities_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	return data;

	//return bonobo::mesh_data();
}
bonobo::mesh_data
parametric_shapes::createSphereBatch2(float const radius,
	unsigned int const longitude_split_count,
	unsigned int const latitude_split_count, int particleNum, std::vector<glm::vec3>& positions, std::vector<glm::vec3>& velocities)
{

	auto const longtitude_slice_edges_count = longitude_split_count;
	auto const latitude_slice_edges_count = latitude_split_count;
	auto const longtitude_slice_vertices_count = longtitude_slice_edges_count + 1u;
	auto const latitude_slice_vertices_count = latitude_slice_edges_count + 1u;
	auto const vertices_nb = longtitude_slice_vertices_count * latitude_slice_vertices_count;

	auto vertices = std::vector<glm::vec3>(vertices_nb);

	float d_theta = glm::two_pi<float>() / static_cast<float>(longtitude_slice_edges_count); // 纬度?方向角度间隔,对应赤道一圈被分割成无数份的角度
	float d_phi = glm::pi<float>() / static_cast<float>(latitude_slice_edges_count); // 经度?方向角度间隔，围绕南北极被分割成无数份的角度

	size_t index = 0u;
	float theta = 0.0f;
	float phi = 0.0f;

	for (unsigned int i = 0u; i < latitude_slice_vertices_count; ++i) {
		for (unsigned int j = 0u; j < longtitude_slice_vertices_count; ++j) {
			//把当前分割段的角度算出来，简化函数
			float phi = static_cast<float>(i) * d_phi;
			float theta = static_cast<float>(j) * d_theta;
			float const cos_theta = std::cos(theta);
			float const sin_theta = std::sin(theta);
			float const cos_phi = std::cos(phi);
			float const sin_phi = std::sin(phi);

			vertices[index] = glm::vec3(radius * sin_phi * sin_theta, -radius * cos_phi, radius * cos_theta * sin_phi);

			//float u = static_cast<float>(j) / static_cast<float>(longtitude_slice_edges_count);
			//float v = static_cast<float>(i) / static_cast<float>(latitude_slice_edges_count);

			float u = static_cast<float>(j) / (longtitude_slice_vertices_count - 1);
			float v = static_cast<float>(i) / (latitude_slice_vertices_count - 1);
			//float u = theta  / glm::two_pi<float>();
			//float v = phi / glm::pi<float>();
			++index;
		}

	}    //每个四边形有两个三角形，空间就得*2
	auto index_sets = std::vector<glm::uvec3>(2u * longtitude_slice_edges_count * latitude_slice_edges_count);


	//int p = 2u*longtitude_slice_edges_count*latitude_slice_edges_count;

	//遍历球体上的所有四边形，分割成两个三角形，注：定点顺序逆时针代表正面，顺时针反面
	index = 0u;
	for (int i = 0u; i < latitude_slice_edges_count; ++i)
	{
		for (int j = 0u; j < longtitude_slice_edges_count; ++j)
		{
			//order：（i，j）->（i+1，j）->（i，j+1）不会越界因为点比edges多1
			index_sets[index] = glm::uvec3(
				i * longtitude_slice_vertices_count + j,
				(i + 1) * longtitude_slice_vertices_count + j,
				i * longtitude_slice_vertices_count + j + 1);
			++index;
			//order：(i+1,j)->(i+1,j+1)->(i,j+1)
			index_sets[index] = glm::uvec3(
				(i + 1) * longtitude_slice_vertices_count + j,
				(i + 1) * longtitude_slice_vertices_count + j + 1,
				i * longtitude_slice_vertices_count + j + 1);
			++index;

		}
	}

	//同circle_RING,我未做任何修改，可复制粘贴直接使用，应该是类似exercise1 的cpu gpu操作（缓冲区
	bonobo::mesh_data data;
	glGenVertexArrays(1, &data.vao);
	assert(data.vao != 0u);
	glBindVertexArray(data.vao);

	auto const vertices_offset = 0u;
	auto const vertices_size = static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3));
	auto const positions_offset = vertices_size;
	auto const positions_size = static_cast<GLsizeiptr>(positions.size() * sizeof(glm::vec3));
	auto const velocities_offset = positions_offset + positions_size;
	auto const velocities_size = static_cast<GLsizeiptr>(velocities.size() * sizeof(glm::vec3));
	auto const bo_size = static_cast<GLsizeiptr>(vertices_size + positions_size + velocities_size);

	glGenBuffers(1, &data.bo);
	assert(data.bo != 0u);
	glBindBuffer(GL_ARRAY_BUFFER, data.bo);
	glBufferData(GL_ARRAY_BUFFER, bo_size, nullptr, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, vertices_offset, vertices_size, static_cast<GLvoid const*>(vertices.data()));
	glEnableVertexAttribArray(static_cast<unsigned int>(bonobo::shader_bindings::vertices));
	glVertexAttribPointer(static_cast<unsigned int>(bonobo::shader_bindings::vertices), 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid const*>(0x0));

	glBufferSubData(GL_ARRAY_BUFFER, positions_offset, positions_size, static_cast<GLvoid const*>(positions.data()));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(positions_offset));

	glBufferSubData(GL_ARRAY_BUFFER, velocities_offset, velocities_size, static_cast<GLvoid const*>(velocities.data()));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<GLvoid const*>(velocities_offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0u);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	data.indices_nb = static_cast<GLsizei>(index_sets.size() * 3u);
	data.vertices_nb = vertices_nb;

	glGenBuffers(1, &data.ibo);
	assert(data.ibo != 0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_sets.size() * sizeof(glm::uvec3)), reinterpret_cast<GLvoid const*>(index_sets.data()), GL_STATIC_DRAW);

	glBindVertexArray(0u);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);

	//! \todo Implement this function
	return data;
}
