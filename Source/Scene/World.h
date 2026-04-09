#pragma once

#include <d3d11.h>
#include <vector>

#include "Object.h"

class World
{
public:
	World() = default;
	~World() = default;
	World(const World& rhs) = delete;
	World& operator=(const World& rhs) = delete;
	World(World&& rhs) noexcept = default;
	World& operator=(World&& rhs) noexcept = default;

	void LoadWorld(ID3D11Device* device);
	void AddObject(Object&& object);
	size_t GetObjectCount() const;
	Object* GetObjects();

private:
	std::vector<Object> m_objects;

};
