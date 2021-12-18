/*******************************************************************
 ** This code is part of Breakout.
 **
 ** Breakout is free software: you can redistribute it and/or modify
 ** it under the terms of the CC BY 4.0 license as published by
 ** Creative Commons, either version 4 of the License, or (at your
 ** option) any later version.
 ******************************************************************/
#pragma once


#include "graphics/texture.h"
#include "graphics/shader.h"



class SpriteRenderer
{
public:
    // Constructor (inits shaders/shapes)
    SpriteRenderer();
    // Destructor
    ~SpriteRenderer();
    static SpriteRenderer* GetInstance()
    {
        static auto* instance = new SpriteRenderer();
        return instance;
    }
    // Renders a defined quad textured with given sprite
	void DrawTexture(unsigned int textureID, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
	void DrawTexture(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawSprite(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawMask(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawFrameSprite(unsigned int textureID, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawFrameSprite(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    void DrawMapSprite(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(0, 0), GLfloat rotate = 0.0f, GLfloat alpha=0.0f);
	void DrawBitmap(Texture* texture, glm::vec2 position, glm::vec2 size = glm::vec2(1, 1), glm::vec3 color = glm::vec3(1.0f),GLfloat alpha = 0.0f ,bool symmetrical= false);
	void Bind() { m_pShader->Bind(); };
	void UnBind() {	m_pShader->Unbind();};
    void UpdateProjection();
    void ResetDrawCall() { m_DrawCall = 0; }
    int GetDrawCall() { return m_DrawCall; }
private:
    // Render state
    Shader* m_pShader;
	GLuint VBO;
	GLuint symmetricalVBO;
    GLuint quadVAO;
	GLuint symmetricalQuadVAO;
    // Initializes and configures the quad's buffer and vertex attributes
    void initRenderData();
    int m_DrawCall;
};
#define SPRITE_RENDERER_INSTANCE SpriteRenderer::GetInstance()

void luaopen_sprite_renderer(lua_State* L);
