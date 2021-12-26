#pragma once


#include "shader.h"

class Texture
{
public:
	enum CreationType
	{
		Text,
		Sprite
	};

	Texture();
	Texture(std::string filePath,bool freeImage = false);
	Texture(int width, int height, bool alpha, uint8 *lImageBuffer);
	Texture(const Texture& t);
	Texture(CreationType type, const uint8* buff, int left, int top, int width, int height);

	Texture& operator=(const Texture& t)
	{
		this->m_TextureID = t.m_TextureID;
		this->m_Format = t.m_Format;
		this->m_Width = t.m_Width;
		this->m_Height = t.m_Height;
		this->m_Path = t.m_Path;
		this->m_Left = t.m_Left;
		this->m_Top = t.m_Top;
		return *this;
	}

	~Texture();

	int32 GetWidth() const { return m_Width; }
	int32 GetHeight() const { return m_Height; }
	int GetLeft() const { return m_Left; }
	int GetTop() const { return m_Top; }
	GLuint GetTextureID() const { return m_TextureID; }

	void SetWidth(int w) { m_Width = w; }
	void SetHeight(int h) { m_Height = h; }
	std::string GetTag() { return m_Tag; }
	void SetTag(std::string tag) { m_Tag = tag; }
	std::string GetPath() { return m_Path; }
private:
	GLuint m_TextureID;
	GLint m_Format;
	int32 m_Width, m_Height;
	std::string m_Path;
	int m_Left;
	int m_Top;
	std::string m_Tag;
};

