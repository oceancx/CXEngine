#include "texture.h"
#include "graphics/image.h"
#include "graphics/bitmap.h"
#include "logger.h"
#include <stb_image.h>


Texture::Texture(CreationType type, const uint8* buff,int left,int top, int width, int height)
	:m_TextureID(0),m_Left(left),m_Top(top), m_Width(width), m_Height(height)
{
	if (type == Text)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			width,
			height,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			buff
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
		m_Format = GL_RED;

		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}
}

Texture::Texture() 
{

}

Texture::Texture(const Texture& t)
{
	this->m_TextureID = t.m_TextureID;
	this->m_Format = t.m_Format;
	this->m_Width = t.m_Width;
	this->m_Height = t.m_Height;
	this->m_Path = t.m_Path;
	this->m_Left = t.m_Left;
	this->m_Top = t.m_Top;
}


Texture::Texture(std::string filePath,bool freeImage) 
: m_TextureID(0),
m_Format(0),
m_Width(0),
m_Height(0),
m_Left(0),
m_Top(0),
m_Path(filePath)
{
	uint8* lImageBuffer = nullptr;
	std::string suffix = filePath.substr(filePath.find_last_of("."));
	if (suffix == ".bmp")
	{
		BitmapFile bpFile;
		bool ok =  Bitmap::Load(filePath,bpFile);
		if (ok)
		{
			lImageBuffer = bpFile.imageData;
			m_Format = bpFile.infoHeader.biBitCount == 32 ? GL_RGBA : GL_RGB;
			m_Width = bpFile.infoHeader.biWidth;
			m_Height = bpFile.infoHeader.biHeight;
		}
		else
		{
			cxlog_err("ReadBitmapFile FAILED\n");
			return;
		}
	}
	else
	{
		int  channel = 0;
		stbi_set_unpremultiply_on_load(1);
		stbi_convert_iphone_png_to_rgb(1);
		lImageBuffer = stbi_load(m_Path.c_str(), &m_Width, &m_Height, &channel, 0);
		if (lImageBuffer == NULL)
		{
			cxlog_err("ReadSOIL FAILED - NO IMAGE BUFFER\n");
			return;
		}

		if (channel == 3 ) // RGB
		{
			m_Format = GL_RGB;
		}
		else if (channel == 4) //RGBA
		{
			m_Format = GL_RGBA;
		}
		else
		{
			m_Format = GL_RGB;
		}
	}
	
	//std::cout << "width: " << m_Height << " height: "<< m_Height << std::endl;
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, m_Format, GL_UNSIGNED_BYTE, lImageBuffer);
	
	if (freeImage)
	{
		if (suffix == ".bmp")
		{
			delete lImageBuffer;
		}
		else
		{
			stbi_image_free(lImageBuffer);
			free(lImageBuffer);
		}
		lImageBuffer = nullptr;
	}
}


Texture::Texture(int width,int height,bool alpha,uint8 *lImageBuffer) 
: m_TextureID(0),
m_Format(0),
m_Width(width),
m_Height(height),
m_Left(0),
m_Top(0)
{
	if (alpha)
    {
		m_Format = GL_RGBA;
    }
    else
    {
		m_Format = GL_RGB;
    }    
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, m_Format, GL_UNSIGNED_BYTE, lImageBuffer);

	// delete[] lImageBuffer;
}

Texture::~Texture()
{
	if(m_TextureID != 0)
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}
}
