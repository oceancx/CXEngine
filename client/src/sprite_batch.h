#pragma once 

#include "singleton.h"
#include "graphics/shader.h"
/*
这个类主要用来控制每帧要刷新的Sprite的数量
1. 将所有的SpriteInfo（pos,size,alpha,texture）转换为顶点+Texture
2. 转化后的东西保存在类里面
3. 调用Flush后 画面进行绘制
*/
struct SpriteInfo
{
	glm::vec2 Pos;			
	glm::vec2 Size;	

	float Alpha;
	GLuint TextureID;	

	SpriteInfo()
		:Alpha(1.0f)
	{

	}
};

class SpriteBatch final : public Singleton<SpriteBatch>
{
public:
	friend Singleton<SpriteBatch>;
	
	void Init();
	void Flush();
	void AddSprite(const SpriteInfo* spriteInfo);

private:
	SpriteBatch();
	~SpriteBatch();

	void Begin();
	void End();

	static const uint32 BATCHSIZE = 50;
	static const uint32 VERTEX_AMOUNT = 18;
	static const uint32 UV_AMOUNT = 12;
	static const uint32 FIRST_REAL_ASCII_CHAR = 31;

	std::vector<const SpriteInfo*> m_SpriteQueue;
	std::vector<GLfloat> mVertices;

	GLuint VAO,VBO;
	
	Shader* m_pShader;
		
	glm::mat4 view,projection;   
	GLint modelLoc,viewLoc, projLoc;

};