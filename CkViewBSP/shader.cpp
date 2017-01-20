//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Libraries Include
#include "libraries.h"
// This Include
#include "shader.h"

const bool CShader::Initialize(	const char* _kpcVertSrc,
								const char* _kpcCtrlSrc,
								const char* _kpcEvalSrc,
								const char* _kpcGeomSrc,
								const char* _kpcFragSrc,
								const char* _kpcCompSrc,
								const std::vector<std::string> _kVariableNames)
{
	GLint iResult;
	GLuint uiVertShader;
	GLuint uiCtrlShader;
	GLuint uiEvalShader;
	GLuint uiGeomShader;
	GLuint uiFragShader;
	GLuint uiCompShader;

	const size_t kszErrBufSize = 8192;
	char ErrBuffer[kszErrBufSize];
	GLsizei ErrLength;

	// Vert Shader
	if(_kpcVertSrc)
	{
		uiVertShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(uiVertShader, 1, &_kpcVertSrc, nullptr);
		glCompileShader(uiVertShader);
		glGetShaderiv(uiVertShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiVertShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Ctrl Shader
	if(_kpcCtrlSrc)
	{
		uiCtrlShader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(uiCtrlShader, 1, &_kpcCtrlSrc, nullptr);
		glCompileShader(uiCtrlShader);
		glGetShaderiv(uiCtrlShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiCtrlShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Eval Shader
	if(_kpcEvalSrc)
	{
		uiEvalShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(uiEvalShader, 1, &_kpcEvalSrc, nullptr);
		glCompileShader(uiEvalShader);
		glGetShaderiv(uiEvalShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiEvalShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Geom Shader
	if(_kpcGeomSrc)
	{
		uiGeomShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(uiGeomShader, 1, &_kpcGeomSrc, nullptr);
		glCompileShader(uiGeomShader);
		glGetShaderiv(uiGeomShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiGeomShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Frag Shader
	if(_kpcFragSrc)
	{
		uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(uiFragShader, 1, &_kpcFragSrc, nullptr);
		glCompileShader(uiFragShader);
		glGetShaderiv(uiFragShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiFragShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Comp Shader
	if(_kpcCompSrc)
	{
		uiCompShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(uiCompShader, 1, &_kpcCompSrc, nullptr);
		glCompileShader(uiCompShader);
		glGetShaderiv(uiCompShader, GL_COMPILE_STATUS, &iResult);
		if(iResult != GL_TRUE)
		{
			glGetShaderInfoLog(uiCompShader, kszErrBufSize, &ErrLength, ErrBuffer);
			return(false);
		}
	}

	// Shader Program
	this->m_uiShaderProgram = glCreateProgram();
	if(_kpcVertSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiVertShader);
	}
	if(_kpcCtrlSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiCtrlShader);
	}
	if(_kpcEvalSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiEvalShader);
	}
	if(_kpcGeomSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiGeomShader);
	}
	if(_kpcFragSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiFragShader);
	}
	if(_kpcCompSrc)
	{
		glAttachShader(this->m_uiShaderProgram, uiCompShader);
	}
	glLinkProgram(this->m_uiShaderProgram);
	glGetProgramiv(this->m_uiShaderProgram, GL_LINK_STATUS, &iResult);
	if(iResult != GL_TRUE)
	{
		glGetProgramInfoLog(this->m_uiShaderProgram, kszErrBufSize, &ErrLength, ErrBuffer);
		return(false);
	}

	// Shader Variables
	for(size_t i = 0; i < _kVariableNames.size(); ++i)
	{
		this->m_Variables.insert(std::make_pair(_kVariableNames[i], glGetUniformLocation(this->m_uiShaderProgram, _kVariableNames[i].c_str())));
	}

	return(true);
}

void CShader::UseProgram()
{
	glUseProgram(this->m_uiShaderProgram);
}

const GLint CShader::GetVariable(const char* _kpcVariableName)
{
	return(this->m_Variables.find(_kpcVariableName)->second);
}
