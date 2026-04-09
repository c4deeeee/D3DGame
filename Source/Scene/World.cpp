#include "World.h"

#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{
	constexpr char kModelPath[] = "Resource/Assets/master_miku2/source/master_miku2.fbx";
	constexpr char kTextureRoot[] = "Resource/Assets/master_miku2/textures";

	XMMATRIX ConvertAiMatrixToXmMatrix(const aiMatrix4x4& matrix)
	{
		return XMMATRIX(
			matrix.a1, matrix.b1, matrix.c1, matrix.d1,
			matrix.a2, matrix.b2, matrix.c2, matrix.d2,
			matrix.a3, matrix.b3, matrix.c3, matrix.d3,
			matrix.a4, matrix.b4, matrix.c4, matrix.d4);
	}

	std::string BuildTexturePath(const aiString& texturePath)
	{
		const std::string rawPath = texturePath.C_Str();
		const size_t separatorIndex = rawPath.find_last_of("/\\");
		const std::string fileName = separatorIndex == std::string::npos
			? rawPath
			: rawPath.substr(separatorIndex + 1);

		if (fileName.empty())
		{
			return {};
		}

		return std::string(kTextureRoot) + "/" + fileName;
	}

	bool TryResolveTexturePath(const aiMaterial* material, std::string& resolvedTexturePath)
	{
		if (!material)
		{
			return false;
		}

		const aiTextureType textureTypes[] =
		{
			aiTextureType_BASE_COLOR,
			aiTextureType_DIFFUSE
		};

		for (aiTextureType textureType : textureTypes)
		{
			if (aiGetMaterialTextureCount(material, textureType) == 0)
			{
				continue;
			}

			aiString texturePath;
			if (aiGetMaterialTexture(material, textureType, 0, &texturePath) != AI_SUCCESS)
			{
				continue;
			}

			if (texturePath.length == 0 || texturePath.C_Str()[0] == '*')
			{
				continue;
			}

			resolvedTexturePath = BuildTexturePath(texturePath);
			if (!resolvedTexturePath.empty())
			{
				return true;
			}
		}

		return false;
	}
}

void World::LoadWorld(ID3D11Device* device)
{
	m_objects.clear();

	if (!device)
	{
		return;
	}

	const aiScene* scene = aiImportFile(
		kModelPath,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return;
	}

	std::unique_ptr<Model> model = std::make_unique<Model>();
	size_t loadedPartCount = 0;

	std::function<void(aiNode*, const XMMATRIX&)> loadNode;
	loadNode = [&](aiNode* node, const XMMATRIX& parentTransform)
	{
		if (!node)
		{
			return;
		}

		const XMMATRIX nodeTransform = ConvertAiMatrixToXmMatrix(node->mTransformation);
		const XMMATRIX globalTransform = parentTransform * nodeTransform;

		for (unsigned int meshSlot = 0; meshSlot < node->mNumMeshes; ++meshSlot)
		{
			const unsigned int meshIndex = node->mMeshes[meshSlot];
			if (meshIndex >= scene->mNumMeshes)
			{
				continue;
			}

			const aiMesh* aiMeshPtr = scene->mMeshes[meshIndex];
			if (!aiMeshPtr || aiMeshPtr->mNumVertices == 0 || aiMeshPtr->mNumFaces == 0)
			{
				continue;
			}

			if (aiMeshPtr->mMaterialIndex >= scene->mNumMaterials)
			{
				continue;
			}

			std::string texturePath;
			const aiMaterial* material = scene->mMaterials[aiMeshPtr->mMaterialIndex];
			if (!TryResolveTexturePath(material, texturePath))
			{
				continue;
			}

			std::vector<Mesh::Vertex> vertices;
			vertices.reserve(aiMeshPtr->mNumVertices);

			for (unsigned int vertexIndex = 0; vertexIndex < aiMeshPtr->mNumVertices; ++vertexIndex)
			{
				Mesh::Vertex vertex = {};

				const aiVector3D& sourcePosition = aiMeshPtr->mVertices[vertexIndex];
				const XMVECTOR transformedPosition = XMVector3TransformCoord(
					XMVectorSet(sourcePosition.x, sourcePosition.y, sourcePosition.z, 1.0f),
					globalTransform);
				XMStoreFloat3(&vertex.position, transformedPosition);

				if (aiMeshPtr->HasTextureCoords(0))
				{
					vertex.texture = XMFLOAT2(
						aiMeshPtr->mTextureCoords[0][vertexIndex].x,
						aiMeshPtr->mTextureCoords[0][vertexIndex].y);
				}
				else
				{
					vertex.texture = XMFLOAT2(0.0f, 0.0f);
				}

				vertices.push_back(vertex);
			}

			std::vector<unsigned long> indices;
			indices.reserve(aiMeshPtr->mNumFaces * 3);

			for (unsigned int faceIndex = 0; faceIndex < aiMeshPtr->mNumFaces; ++faceIndex)
			{
				const aiFace& face = aiMeshPtr->mFaces[faceIndex];
				if (face.mNumIndices != 3)
				{
					continue;
				}

				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}

			if (indices.empty())
			{
				continue;
			}

			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
			const std::string meshName = aiMeshPtr->mName.length > 0
				? aiMeshPtr->mName.C_Str()
				: ("Mesh_" + std::to_string(meshIndex));
			if (!mesh->Initialize(device, vertices, indices, meshName))
			{
				continue;
			}

			std::unique_ptr<Texture> texture = std::make_unique<Texture>();
			if (!texture->Initialize(device, texturePath))
			{
				continue;
			}

			ModelPart part;
			part.SetMesh(mesh.release());
			part.SetTexture(texture.release());
			model->AddParts(std::move(part));
			++loadedPartCount;
		}

		for (unsigned int childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
		{
			loadNode(node->mChildren[childIndex], globalTransform);
		}
	};

	loadNode(scene->mRootNode, XMMatrixIdentity());

	if (loadedPartCount > 0)
	{
		Object object;
		object.SetLocation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		object.SetRotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		object.SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
		object.SetModel(std::move(model));
		AddObject(std::move(object));
	}

	aiReleaseImport(scene);
}

void World::AddObject(Object&& object)
{
	m_objects.push_back(std::move(object));
}

size_t World::GetObjectCount() const
{
	return m_objects.size();
}

Object* World::GetObjects()
{
	return m_objects.data();
}
