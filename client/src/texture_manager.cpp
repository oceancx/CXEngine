#include "texture_manager.h"

TextureManager::TextureManager()
{

}

TextureManager::~TextureManager()
{

}

Texture* TextureManager::LoadTexture(String path, int w, int h, bool alpha, uint8* imageData)
{
	auto it = m_Textures.find(path);
	if (it != m_Textures.end())
	{
		return it->second;
	}
	else
	{
		m_Textures[path] = new Texture(w, h, alpha, imageData);
		return m_Textures[path];
	}
}

void TextureManager::LoadTexture(String path)
{
	if (m_Textures.find(path) == m_Textures.end())
	{
		m_Textures[path] = new Texture(path);
	}
	else
	{
		return;
	}
}

void TextureManager::UnloadTexture(String path)
{
	if (m_Textures.find(path) == m_Textures.end())
	{
		return;
	}
	else
	{
		delete m_Textures[path];
		m_Textures[path] = nullptr;
		m_Textures.erase(path);

	}
}

Texture* TextureManager::GetTexture(String path)
{
	if (m_Textures.find(path) != m_Textures.end())
	{
		return m_Textures[path];
	}
	else
	{
		return nullptr;
	}
}


void TextureManager::ClearAll()
{
	for (auto& it : m_Textures)
	{
		delete it.second;
		it.second = nullptr;

	}

	m_Textures.clear();
}

int TextureManager::GetTextureCount()
{
	return (int)m_Textures.size();
}
