#pragma once



/*
封装OpenGL内部的Shader
暴露出一些方法用来控制OpenGL内部状态
*/
class Shader
{
public:
	Shader();
	Shader(const char* vertPath, const char* fragPath, const char* geomPath = nullptr);
	~Shader();

	void Init(const std::string vertCode, const  std::string fragCode, const  std::string geomCode = "");

	void Bind();

	void Unbind();

	const GLuint GetProgramID() const;
	
	// utility uniform functions
	// ------------------------------------------------------------------------
	void SetBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), (int)value);
	}
	// ------------------------------------------------------------------------
	void SetInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_ProgramID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void SetFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_ProgramID, name.c_str()), value);
	}
	// ------------------------------------------------------------------------
	void SetVec2(const std::string &name, const glm::vec2 &value) const
	{
		glUniform2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &value[0]);
	}
	void SetVec2(const std::string &name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(m_ProgramID, name.c_str()), x, y);
	}
	// ------------------------------------------------------------------------
	void SetVec3(const std::string &name, const glm::vec3 &value) const
	{
		glUniform3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &value[0]);
	}
	void SetVec3(const std::string &name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(m_ProgramID, name.c_str()), x, y, z);
	}
	// ------------------------------------------------------------------------
	void SetVec4(const std::string &name, const glm::vec4 &value) const
	{
		glUniform4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, &value[0]);
	}
	void SetVec4(const std::string &name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(m_ProgramID, name.c_str()), x, y, z, w);
	}
	// ------------------------------------------------------------------------
	void SetMat2(const std::string &name, const glm::mat2 &mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void SetMat3(const std::string &name, const glm::mat3 &mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	void SetMat4(const std::string &name, const glm::mat4 &mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	

private:
	 void checkCompileErrors(GLuint shader, std::string type);

	GLuint m_ProgramID;
		
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_GeometryShader;
	std::string m_VertexPath;
	std::string m_FragmentPath;
	std::string m_GeometryPath;
}; 