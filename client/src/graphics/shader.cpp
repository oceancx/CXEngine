#include "shader.h"
#include "file_system.h"

Shader::Shader()
{

}

Shader::Shader(const char* vertPath, const char* fragPath, const char* geomPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	// ensures ifstream objects can throw exceptions:
	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try 
	{
		m_VertexPath = FileSystem::GetShaderPath(vertPath);
		m_FragmentPath = FileSystem::GetShaderPath(fragPath);

		vShaderFile.open(m_VertexPath.c_str());
		fShaderFile.open(m_FragmentPath.c_str());
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();		

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();			

		if (geomPath != nullptr)
		{
			m_GeometryPath = FileSystem::GetShaderPath(geomPath);
			gShaderFile.open(m_GeometryPath.c_str());
			std::stringstream gShaderStream;
			gShaderStream << gShaderFile.rdbuf();
			gShaderFile.close();
			geometryCode = gShaderStream.str();
		}

		Init(vertexCode, fragmentCode, geometryCode);
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ:\n" << e.what() << std::endl;
	}
}

void Shader::Init(const std::string vertCode,const std::string fragCode,const std::string gemoCode)
{
	const GLchar* vertexShaderSource  = vertCode.c_str();
	const GLchar* fragmentShaderSource  = fragCode.c_str();

	m_VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_VertexShader, 1, &vertexShaderSource, NULL);	
	glCompileShader(m_VertexShader);
	checkCompileErrors(m_VertexShader, "VERTEX");

	m_FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_FragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(m_FragmentShader);
	checkCompileErrors(m_FragmentShader, "FRAGMENT");

	if(!gemoCode.empty())
	{
		const GLchar * geometryShaderSource = gemoCode.c_str();
		m_GeometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(m_GeometryShader, 1, &geometryShaderSource, NULL);
		glCompileShader(m_GeometryShader);
		checkCompileErrors(m_GeometryShader, "GEOMETRY");
	}

	m_ProgramID = glCreateProgram();
	glAttachShader(m_ProgramID, m_VertexShader);
	glAttachShader(m_ProgramID, m_FragmentShader);
	if(!gemoCode.empty())
	{
		glAttachShader(m_ProgramID, m_GeometryShader);
	}

	glLinkProgram(m_ProgramID);
	checkCompileErrors(m_ProgramID, "PROGRAM");

	glDeleteShader(m_VertexShader);
	glDeleteShader(m_FragmentShader);
	if(!gemoCode.empty())
	{
		glDeleteShader(m_GeometryShader);
	}
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if(type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
		}
	}
}

void Shader::Bind()
{
	glUseProgram(m_ProgramID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

const GLuint Shader::GetProgramID() const
{
	return m_ProgramID;
}

Shader::~Shader()
{
	//glDeleteProgram(m_ProgramID);
}