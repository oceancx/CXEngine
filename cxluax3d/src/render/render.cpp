#include "render/render.h"
#include <window.h>
#include <logger.h>
#include <render\shader.h>


namespace cxluax3d
{
	unsigned int m_Fbo;
	unsigned int m_Rbo;
	unsigned int m_TextureColor;

	Shader* g_pShader = nullptr;


	const char* simple_3d_fs = R"(#version 330 core
out vec4 FragColor;
  
void main()
{
    FragColor = vec4(1.0,0.6,0.8, 1.0);
})";

	const char* simple_3d_vs = R"(#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main()
{
	//gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = transform* vec4(aPos, 1.0);
})";

	void iw_function_to_select_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
		glDisable(GL_BLEND);
	}

	void iw_function_to_restore_shader_or_blend_state(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
		glEnable(GL_BLEND);
	}


	void render_init()
	{
		glGenFramebuffers(1, &m_Fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

		int screenWidth = WINDOW_INSTANCE->GetWidth();
		int screenHeight = WINDOW_INSTANCE->GetHeight();
		glGenTextures(1, &m_TextureColor);
		glBindTexture(GL_TEXTURE_2D, m_TextureColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColor, 0);

		glGenRenderbuffers(1, &m_Rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cxlog_err("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		g_pShader = new Shader();
		g_pShader->Init(simple_3d_vs, simple_3d_fs);


	}

	void render_deinit()
	{

	}

	float line_pts[] = {
		0,0,0,0.75f,0.75f,0.75f
	};


	float ori_pts[] = {
		0,0,0
	};



	void render_update()
	{

		float dt = glfwGetTime();
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		g_pShader->Bind();

		glm::mat4 transform = glm::mat4(1);
		transform = glm::translate(transform, glm::vec3(0.1f, 0, 0));

		g_pShader->SetMat4("transform", transform);

		unsigned int vao = 0;
		glGenVertexArrays(1, &vao);

		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);

			
		glBindVertexArray(vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(line_pts), line_pts, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		glBindVertexArray(0);
		

		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);


		transform = glm::mat4(1);
		g_pShader->SetMat4("transform", transform);
		unsigned int ori_vbo = 0;
		glGenBuffers(1, &ori_vbo);

		unsigned int ori_vao = 0;
		glGenVertexArrays(1, &ori_vao);


		glBindVertexArray(ori_vao);
			glBindBuffer(GL_ARRAY_BUFFER, ori_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(ori_pts), ori_pts, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		glBindVertexArray(0);


		glBindVertexArray(ori_vao);
		glDrawArrays(GL_POINTS, 0, 1);
		glBindVertexArray(0);

		//// texture coord attribute
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		//glEnableVertexAttribArray(1);

		g_pShader->Unbind();

	}

	void render_begin(int w, int h)
	{
		auto cs_pos = ImGui::GetCursorPos();
		auto css_pos = ImGui::GetCursorScreenPos();
		ImGui::GetWindowDrawList()->AddCallback(iw_function_to_select_shader_or_blend_state, nullptr);
		ImGui::GetWindowDrawList()->AddImage((void*)(uint64_t)m_TextureColor, css_pos, ImVec2(css_pos.x + w, css_pos.y + h), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::GetWindowDrawList()->AddCallback(iw_function_to_restore_shader_or_blend_state, nullptr);
		ImGui::SetCursorPos(cs_pos);
		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	}

	void render_end()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	}

	void render_on_frame_buffer_size_changed(int w, int h)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

		int screenWidth = WINDOW_INSTANCE->GetWidth();
		int screenHeight = WINDOW_INSTANCE->GetHeight();
		glGenTextures(1, &m_TextureColor);
		glBindTexture(GL_TEXTURE_2D, m_TextureColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColor, 0);

		glGenRenderbuffers(1, &m_Rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			cxlog_err("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
		}
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glViewport(0, 0, w, h);
	}

}
