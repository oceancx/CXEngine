#include "sprite_batch.h"
#include "file_system.h"
#include <array>

SpriteBatch::SpriteBatch()
:Singleton<SpriteBatch>()
,m_SpriteQueue()
,m_pShader(nullptr)
{

}

SpriteBatch::~SpriteBatch(void)
{
	delete m_pShader;
}

void SpriteBatch::Init()
{
	m_pShader = new Shader("MvpVertexShader.vs", "MvpFragmentShader.fs");
	m_pShader->Bind();
	projection = glm::ortho(0.0f, 800.0f,600.0f,0.0f, -100.0f, 100.0f);
	view = glm::mat4();
   // Get the uniform locations
	modelLoc = glGetUniformLocation(m_pShader->GetProgramID(), "model");
	viewLoc = glGetUniformLocation(m_pShader->GetProgramID(), "view");
	projLoc = glGetUniformLocation(m_pShader->GetProgramID(), "projection");
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	m_pShader->Unbind();
}

void SpriteBatch::Flush()
{
	m_pShader->Bind();
	Begin();
	End();
	m_pShader->Unbind();
	m_SpriteQueue.clear();
}

void SpriteBatch::AddSprite(const SpriteInfo* spriteInfo)
{
	m_SpriteQueue.push_back(spriteInfo);
}

void SpriteBatch::Begin()
{
	/*
	初始化顶点数据等
	*/
	mVertices.clear();
	for(int i=0;i<m_SpriteQueue.size();i++)
	{
		if(i>=BATCHSIZE)break;
		const SpriteInfo* info = m_SpriteQueue[i];

		float x,y,width,height;
		float alpha = info->Alpha;
		x = info->Pos.x;
		y = info->Pos.y;
		width = info->Size.x;
		height = info->Size.y;

		//TL
		mVertices.push_back(x);			//x
		mVertices.push_back(y);			//y
		mVertices.push_back(0);			//z
		mVertices.push_back(0);			//tx
		mVertices.push_back(0);			//ty
		mVertices.push_back(alpha);		//alpha

		//TR
		mVertices.push_back(x+width);	//x
		mVertices.push_back(y);			//y
		mVertices.push_back(0);			//z
		mVertices.push_back(1);			//tx
		mVertices.push_back(0);			//ty
		mVertices.push_back(alpha);		//alpha


		//BL
		mVertices.push_back(x);			//x
		mVertices.push_back(y+height);	//y
		mVertices.push_back(0);			//z
		mVertices.push_back(0);			//tx
		mVertices.push_back(1);			//ty
		mVertices.push_back(alpha);		//alpha

		//BL
		mVertices.push_back(x);			//x
		mVertices.push_back(y+height);			//y
		mVertices.push_back(0);			//z
		mVertices.push_back(0);			//tx
		mVertices.push_back(1);			//ty
		mVertices.push_back(alpha);			//alpha

		//TR
		mVertices.push_back(x+width);			//x
		mVertices.push_back(y);			//y
		mVertices.push_back(0);			//z
		mVertices.push_back(1);			//tx
		mVertices.push_back(0);			//ty
		mVertices.push_back(alpha);			//alpha

		//BR
		mVertices.push_back(x+width);			//x
		mVertices.push_back(y+height);			//y
		mVertices.push_back(0);			//z
		mVertices.push_back(1);			//tx
		mVertices.push_back(1);			//ty
		mVertices.push_back(alpha);			//alpha
	}
	
	
	/*std::array<GLfloat, mVertices.size()> vertices;
	for(int i=0;i<mVertices.size();i++)
	{
		vertices[i] = mVertices[i];
	}*/

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mVertices.size(), &mVertices[0], GL_STATIC_DRAW);

	
    // Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
    // TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO

    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void SpriteBatch::End()
{
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (GLfloat*) (&view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, (GLfloat*)(&projection));

	glBindVertexArray(VAO);
	int triangles = 0;
	for(const SpriteInfo* info : m_SpriteQueue)
	{
		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, info->TextureID);
		glUniform1i(glGetUniformLocation(m_pShader->GetProgramID(), "textureSampler"), 0);
		 // Calculate the model matrix for each object and pass it to shader before drawing
		glm::mat4 model;
		glm::vec3 s(0.3,0.3,0);
		model = glm::scale(model,s);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE,  (GLfloat*)(&model));
		glDrawArrays(GL_TRIANGLES, triangles , triangles+6);
		triangles+=6;     
	}
	glBindVertexArray(0);
	
}





