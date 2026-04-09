#include "InputManager.h"

void InputManager::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}

	return;
}

void InputManager::KeyDown(unsigned int input)
{
	m_keys[input] = true;
	return;
}

void InputManager::KeyUp(unsigned int input)
{
	m_keys[input] = false;
	return;
}

bool InputManager::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}