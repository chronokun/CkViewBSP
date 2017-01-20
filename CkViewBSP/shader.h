//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __SHADER_H__
#define __SHADER_H__

class CShader
{
	// Member Variables
protected:
	std::map<std::string, GLint> m_Variables;

public:
	GLuint m_uiShaderProgram;

	// Member Functions
protected:
	//

public:
	const bool Initialize(	const char* _kpcVertSrc,
							const char* _kpcCtrlSrc,
							const char* _kpcEvalSrc,
							const char* _kpcGeomSrc,
							const char* _kpcFragSrc,
							const char* _kpcCompSrc,
							const std::vector<std::string> _kVariableNames);
	void Terminate();
	void UseProgram();
	const GLint GetVariable(const char* _kpcVariableName);

};

#endif