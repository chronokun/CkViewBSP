//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Library Include
#include "libraries.h"
// This Include
#include "renderer.h"
// Local Includes
#include "surface.h"
#include "enginecore.h"
#include "client.h"
#include "utilities.h"
#include "window.h"
#include "qbsp.h"
#include "timer.h"
// NanoVG
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"

void __stdcall DebugHandler(	GLenum _eSource,
								GLenum _eType,
								GLuint _uiID,
								GLenum _eSeverity,
								GLsizei _szLength,
								const GLchar* _kpcMessage,
								const void* _pUserParam)
{
	//
}

const bool CRenderer::SetupDC(const HWND _khWnd)
{
	bool bSuccess = false;
	int iFormat;
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
		1,                     // version number  
		PFD_DRAW_TO_WINDOW |   // support window  
		PFD_SUPPORT_OPENGL |   // support OpenGL  
		PFD_DOUBLEBUFFER,      // double buffered  
		PFD_TYPE_RGBA,         // RGBA type  
		24,                    // 24-bit color depth  
		0, 0, 0, 0, 0, 0,      // color bits ignored  
		0,                     // no alpha buffer  
		0,                     // shift bit ignored  
		0,                     // no accumulation buffer  
		0, 0, 0, 0,            // accum bits ignored  
		32,                    // 32-bit z-buffer  
		0,                     // no stencil buffer  
		0,                     // no auxiliary buffer  
		PFD_MAIN_PLANE,        // main layer  
		0,                     // reserved  
		0, 0, 0                // layer masks ignored
	};

	this->m_hDC = GetDC(_khWnd);

	if(this->m_hDC)
	{
		iFormat = ChoosePixelFormat(this->m_hDC, &pfd);
		if(SetPixelFormat(this->m_hDC, iFormat, &pfd))
		{
			bSuccess = true;
		}
	}

	return(bSuccess);
}

const bool CRenderer::SetupGLRC()
{
	bool bSuccess = false;
	HGLRC hTempGLRC;

	const int kiAttributeList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if defined(DEBUG) || defined(_DEBUG)
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif
		,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0, 0
	};

	// Create temp context
	hTempGLRC = wglCreateContext(this->m_hDC);
	// Make-current temp context
	wglMakeCurrent(this->m_hDC, hTempGLRC);
	// Get 'wglCreateContextAttibsARB' function-pointer
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	// Delete temp context
	wglMakeCurrent(this->m_hDC, NULL);
	wglDeleteContext(hTempGLRC);

	// Create real context
	if(wglCreateContextAttribsARB != nullptr)
	{
		this->m_hGLRC = wglCreateContextAttribsARB(this->m_hDC, 0, kiAttributeList);
		if(this->m_hGLRC != NULL)
		{
			// Make-current real context
			if(wglMakeCurrent(this->m_hDC, this->m_hGLRC))
			{
				if(this->SetupGLFunctions())
				{
					bSuccess = true;
				}
			}
		}
	}

	return(bSuccess);
}

const bool CRenderer::SetupGLFunctions()
{
	return(LoadGLFunctions() == 0);
}

CRenderer::~CRenderer()
{
	//
}

const bool CRenderer::Initialize(const HWND _khWnd)
{
	bool bSuccess = false;

	this->m_hWnd = _khWnd;

	if(!this->SetupDC(_khWnd))
	{
		return(false);
	}
	if(!this->SetupGLRC())
	{
		return(false);
	}
	if(!wglSwapIntervalEXT(0))
	{
		return(false);
	}
	if(!this->SetupShaders())
	{
		return(false);
	}
	// Set some things
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);

#if defined(DEBUG) || defined(_DEBUG)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugHandler, nullptr);
#endif

	glClearColor(0.f, 0.f, 0.f, 1.0f);

	glEnable(GL_FRAMEBUFFER_SRGB);

	//

	#if defined(DEBUG) || defined(_DEBUG)
	m_pNVG = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
#else
	m_pNVG = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
#endif
	if(m_pNVG == nullptr)
	{
		return(false);
	}
	nvgCreateFont(m_pNVG, "sans", "fonts/PetMe128.ttf");

	//

	this->m_FrameTimes.resize(4096);
	for(size_t i = 0; i < this->m_FrameTimes.size(); i++)
	{
		this->m_FrameTimes[i] = -1.f;
	}
	this->m_FrameNumber = 0;
	this->m_fLastFrameTime = (float)CEngineCore::GetInstance().m_pTimer->GetElapsedTime();

	m_bInitializedBufferObjects = false;
	
	return(true);
}

void CRenderer::Draw(const float _kfExtrapolationTime)
{
	glViewport(0, 0, (int)CEngineCore::GetInstance().m_pWindow->m_iWidth, (int)CEngineCore::GetInstance().m_pWindow->m_iHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	int iWidth = (int)CEngineCore::GetInstance().m_pWindow->m_iWidth;
	int iHeight = (int)CEngineCore::GetInstance().m_pWindow->m_iHeight;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
	this->DrawSurfaces();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	nvgBeginFrame(this->m_pNVG, iWidth, iHeight, 1.0f);

	nvgFontSize(this->m_pNVG, 8.0f);
	nvgFontFace(this->m_pNVG, "sans");
	nvgTextAlign(this->m_pNVG, NVG_ALIGN_LEFT|NVG_ALIGN_TOP);

	float fCurrentTime = (float)CEngineCore::GetInstance().m_pTimer->GetElapsedTime();
	int iFrameRate = 0;
	float fFrameTime = fCurrentTime - this->m_fLastFrameTime;
	this->m_fLastFrameTime = fCurrentTime;
	for(size_t i = 0; i < this->m_FrameTimes.size(); i++)
	{
		if(this->m_FrameTimes[i] > (fCurrentTime-1.0f))
		{
			iFrameRate++;
		}
	}

	char Buffer[6][256];

	sprintf_s(Buffer[0], "Culled by PVS: %d", this->m_iCulledByPVS);
	sprintf_s(Buffer[1], "Culled by frustum: %d", this->m_iCulledByFrustum);
	sprintf_s(Buffer[2], "Visible: %d", this->m_iNotCulled);
	sprintf_s(Buffer[3], "FPS: %d", iFrameRate);
	sprintf_s(Buffer[4], "Frametime: %fms", fFrameTime*1000.f);
	sprintf_s(Buffer[5], "Reciprocal: %f", 1.f/fFrameTime);

	nvgFillColor(this->m_pNVG, nvgRGBA(255, 255, 255, 255));
	nvgText(this->m_pNVG, (float)10, (float)10, Buffer[0], nullptr);
	nvgText(this->m_pNVG, (float)10, (float)20, Buffer[1], nullptr);
	nvgText(this->m_pNVG, (float)10, (float)30, Buffer[2], nullptr);
	nvgText(this->m_pNVG, (float)10, (float)40, Buffer[3], nullptr);
	nvgText(this->m_pNVG, (float)10, (float)50, Buffer[4], nullptr);
	nvgText(this->m_pNVG, (float)10, (float)60, Buffer[5], nullptr);

	nvgEndFrame(this->m_pNVG);

	glFinish();
	//
	this->m_FrameTimes[this->m_FrameNumber] = (float)CEngineCore::GetInstance().m_pTimer->GetElapsedTime();
	this->m_FrameNumber = (this->m_FrameNumber+1)%this->m_FrameTimes.size();
}

void CRenderer::DrawSurfaces()
{
	CClient* pClient = CEngineCore::GetInstance().m_pClient;

	TMatrix4f VTranslation = TranslationMatrix(TMatrix4f(), ScalarMultiply(TVector3f(), pClient->m_CameraPosition, -1.0f));

	TMatrix4f VOrientation = RotationMatrix(TMatrix4f(), pClient->m_CameraOrientation);
	TMatrix4f V = Multiply(TMatrix4f(), VTranslation, VOrientation);

	const float kfAspectRatio = (float)CEngineCore::GetInstance().m_pWindow->m_iWidth / (float)CEngineCore::GetInstance().m_pWindow->m_iHeight;

	TMatrix4f P = PerspectiveMatrix(TMatrix4f(), (s_kfPi / 2.0f), kfAspectRatio, 1.0f, 8192.0f);

	TMatrix4f VP = Multiply(TMatrix4f(), V, P);

	m_iCulledByFrustum = 0;
	m_iCulledByPVS = 0;
	if(pClient->m_bDoPVS)
	{
		int32_t iLeaf = FindLeaf(pClient->m_CameraPosition);
		if(iLeaf != m_iCurrentLeaf)
		{
			m_iCurrentLeaf = iLeaf;
			//
			memset(this->m_LeafVisibility.data(), 0, this->m_LeafVisibility.size()*sizeof(uint8_t));
			FindVisibleLeafs(iLeaf);
		}
		memset(this->m_FaceVisibility.data(), 0, this->m_FaceVisibility.size()*sizeof(uint8_t));
		this->m_iVisibleLeafCount = 0;
		this->m_iVisibleFaceCount = 0;
		for(size_t i = 0; i < this->m_VisibleLeafs.size(); i++)
		{
			const size_t kByte = i/8;
			const size_t kBit = ((size_t)0x01 << (size_t)(i%8));
			if((this->m_LeafVisibility[kByte] & kBit) != 0)
			{
				if(IsVisibleFromFrustum((int32_t)i))
				{
					this->m_VisibleLeafs[this->m_iVisibleLeafCount] = (int32_t)i;
					this->m_iVisibleLeafCount++;
				}
				else
				{
					m_iCulledByFrustum++;
				}
			}
			else
			{
				m_iCulledByPVS++;
			}
		}
		for(int32_t i = 0; i < this->m_iVisibleLeafCount; i++)
		{
			GetLeafFaces(this->m_VisibleLeafs[i]);
		}
	}
	else
	{
		m_iCurrentLeaf = 0;
		this->m_iVisibleFaceCount = (int32_t)pClient->m_BSP.m_Faces.size();
		for(int32_t i = 0; i < this->m_iVisibleFaceCount; i++)
		{
			this->m_VisibleFaces[i] = i;
		}
		this->m_iVisibleLeafCount = (int32_t)pClient->m_BSP.m_Leafs.size();
	}
	m_iNotCulled = this->m_iVisibleLeafCount;

	glBindVertexArray(this->m_uiVAO);
	const TQBSP& krBSP = CEngineCore::GetInstance().m_pClient->m_BSP;
	//
	CShader& rPolygonShader = CEngineCore::GetInstance().m_pRenderer->m_Shaders.find("polygon")->second;
	const GLint iPolyV = rPolygonShader.GetVariable("uni_v");
	const GLint iPolyP = rPolygonShader.GetVariable("uni_p");
	const GLint iPolyVP = rPolygonShader.GetVariable("uni_vp");
	const GLint iPolyCamera = rPolygonShader.GetVariable("uni_cameraposition");
	//
	CShader& rPatchShader = CEngineCore::GetInstance().m_pRenderer->m_Shaders.find("patch")->second;
	const GLint iPatchV = rPatchShader.GetVariable("uni_v");
	const GLint iPatchP = rPatchShader.GetVariable("uni_p");
	const GLint iPatchVP = rPatchShader.GetVariable("uni_vp");
	const GLint iPatchCamera = rPatchShader.GetVariable("uni_cameraposition");

	if(pClient->m_bWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	for(int32_t n = 0; n < this->m_iVisibleFaceCount; n++)
	{
		int32_t i = this->m_VisibleFaces[n];
		if(krBSP.m_Faces[i].m_iType == 1) // Polygons
		{
			glBindVertexArray(this->m_uiVAO);
			rPolygonShader.UseProgram();
			glUniformMatrix4fv(iPolyV, 1, GL_FALSE, (float*)&V);
			glUniformMatrix4fv(iPolyP, 1, GL_FALSE, (float*)&P);
			glUniformMatrix4fv(iPolyVP, 1, GL_FALSE, (float*)&VP);
			glUniform3fv(iPolyCamera, 1, (float*)&pClient->m_CameraPosition);
			if(krBSP.m_Faces[i].m_iLightmapIndex >= 0)
			{
				glBindTexture(GL_TEXTURE_2D, this->m_LightmapTextures[krBSP.m_Faces[i].m_iLightmapIndex]);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glDrawElementsBaseVertex(GL_TRIANGLES, krBSP.m_Faces[i].m_iNumMeshVertices, GL_UNSIGNED_INT, &krBSP.m_MeshVerts[krBSP.m_Faces[i].m_iMeshVertex], krBSP.m_Faces[i].m_iVertex);
		}
		else if(krBSP.m_Faces[i].m_iType == 2) // Patches
		{
			rPatchShader.UseProgram();
			glUniformMatrix4fv(iPatchV, 1, GL_FALSE, (float*)&V);
			glUniformMatrix4fv(iPatchP, 1, GL_FALSE, (float*)&P);
			glUniformMatrix4fv(iPatchVP, 1, GL_FALSE, (float*)&VP);
			glUniform3fv(iPatchCamera, 1, (float*)&pClient->m_CameraPosition);
			//
			int iPatchWidth = (krBSP.m_Faces[i].m_Size[0]-1)/2;
			int iPatchHeight = (krBSP.m_Faces[i].m_Size[1]-1)/2;
			int iPatchCount = iPatchWidth * iPatchHeight;
			//
			//glDisable(GL_CULL_FACE);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glBindVertexArray(this->m_uiPatchVAO);
			glPatchParameteri(GL_PATCH_VERTICES, 1);
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->m_uiPatchBufferSSBO);
			if(krBSP.m_Faces[i].m_iLightmapIndex >= 0)
			{
				glBindTexture(GL_TEXTURE_2D, this->m_LightmapTextures[krBSP.m_Faces[i].m_iLightmapIndex]);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glDrawArrays(GL_PATCHES, krBSP.m_Faces[i].m_iMeshVertex, iPatchCount);
			//iPatchesRendered += iPatchCount;
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//glEnable(GL_CULL_FACE);
			//glDrawElementsBaseVertex(GL_PATCHES, Indices.size(), GL_UNSIGNED_INT, Indices.data(), krBSP.m_Faces[i].m_iVertex);
		}
		else if(krBSP.m_Faces[i].m_iType == 3) // Meshes
		{
			glBindVertexArray(this->m_uiVAO);
			rPolygonShader.UseProgram();
			float Normal[3] = {krBSP.m_Faces[i].m_Normal[0], krBSP.m_Faces[i].m_Normal[2], -krBSP.m_Faces[i].m_Normal[1]};
			glUniformMatrix4fv(iPolyV, 1, GL_FALSE, (float*)&V);
			glUniformMatrix4fv(iPolyP, 1, GL_FALSE, (float*)&P);
			glUniformMatrix4fv(iPolyVP, 1, GL_FALSE, (float*)&VP);
			glUniform3fv(iPolyCamera, 1, (float*)&pClient->m_CameraPosition);
			if(krBSP.m_Faces[i].m_iLightmapIndex >= 0)
			{
				glBindTexture(GL_TEXTURE_2D, this->m_LightmapTextures[krBSP.m_Faces[i].m_iLightmapIndex]);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			glDrawElementsBaseVertex(GL_TRIANGLES, krBSP.m_Faces[i].m_iNumMeshVertices, GL_UNSIGNED_INT, &krBSP.m_MeshVerts[krBSP.m_Faces[i].m_iMeshVertex], krBSP.m_Faces[i].m_iVertex);
		}
	}
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CRenderer::BufferSwap()
{
	SwapBuffers(this->m_hDC);
}

void CRenderer::Terminate()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(this->m_hGLRC);
	ReleaseDC(this->m_hWnd, this->m_hDC);
}

const bool CRenderer::SetupShaders()
{
	// Polygon Shader
	std::vector<char> PolygonVert;
	std::vector<char> PolygonFrag;
	if(!(LoadFileToVector(PolygonVert, "shaders/polygon_vert.glsl") && LoadFileToVector(PolygonFrag, "shaders/polygon_frag.glsl")))
	{
		return(false);
	}
	PolygonVert.push_back('\0');
	PolygonFrag.push_back('\0');

	std::vector<std::string> PolygonUniforms{"uni_v", "uni_p", "uni_vp", "uni_cameraposition"};

	CShader PolygonShader;
	if(!PolygonShader.Initialize(	PolygonVert.data(),
									nullptr,
									nullptr,
									nullptr,
									PolygonFrag.data(),
									nullptr,
									PolygonUniforms))
	{
		return(false);
	}

	this->m_Shaders.insert(std::pair<std::string, CShader>("polygon", PolygonShader));

	// Patch Shader
	std::vector<char> PatchVert;
	std::vector<char> PatchCtrl;
	std::vector<char> PatchEval;
	std::vector<char> PatchFrag;
	if(!(	LoadFileToVector(PatchVert, "shaders/patch_vert.glsl")
		 && LoadFileToVector(PatchCtrl, "shaders/patch_ctrl.glsl")
		 && LoadFileToVector(PatchEval, "shaders/patch_eval.glsl")
		 && LoadFileToVector(PatchFrag, "shaders/patch_frag.glsl")))
	{
		return(false);
	}
	PatchVert.push_back('\0');
	PatchCtrl.push_back('\0');
	PatchEval.push_back('\0');
	PatchFrag.push_back('\0');

	std::vector<std::string> PatchUniforms{"uni_v", "uni_p", "uni_vp", "uni_cameraposition"};

	CShader PatchShader;
	if(!PatchShader.Initialize(	PatchVert.data(),
								PatchCtrl.data(),
								PatchEval.data(),
								nullptr,
								PatchFrag.data(),
								nullptr,
								PatchUniforms))
	{
		return(false);
	}

	this->m_Shaders.insert(std::pair<std::string, CShader>("patch", PatchShader));

	return(true);
}

void CRenderer::LoadMap()
{
	TQBSP& rQBSP = CEngineCore::GetInstance().m_pClient->m_BSP;

	this->m_VisibleFaces.resize(rQBSP.m_Faces.size());
	this->m_VisibleLeafs.resize(rQBSP.m_Leafs.size());
	this->m_FaceVisibility.resize((rQBSP.m_Faces.size()/8)+1);
	this->m_LeafVisibility.resize((rQBSP.m_Leafs.size()/8)+1);

	std::vector<TVector3f> Positions(rQBSP.m_Vertices.size());
	std::vector<TVector2f> TexCoords(rQBSP.m_Vertices.size());
	std::vector<TVector3f> Normals(rQBSP.m_Vertices.size());
	std::vector<TVector2f> Lightmaps(rQBSP.m_Vertices.size());
	std::vector<TVector3f> Colors(rQBSP.m_Vertices.size());
	for(size_t i = 0; i < rQBSP.m_Vertices.size(); i++)
	{
		Positions[i].m_fX = rQBSP.m_Vertices[i].m_Position[0];
		Positions[i].m_fY = rQBSP.m_Vertices[i].m_Position[2];
		Positions[i].m_fZ = -rQBSP.m_Vertices[i].m_Position[1];

		TexCoords[i].m_fX = rQBSP.m_Vertices[i].m_TexCoord[0][0];
		TexCoords[i].m_fY = rQBSP.m_Vertices[i].m_TexCoord[0][1];

		Normals[i].m_fX = rQBSP.m_Vertices[i].m_Normal[0];
		Normals[i].m_fY = rQBSP.m_Vertices[i].m_Normal[2];
		Normals[i].m_fZ = -rQBSP.m_Vertices[i].m_Normal[1];

		Lightmaps[i].m_fX = rQBSP.m_Vertices[i].m_TexCoord[1][0];
		Lightmaps[i].m_fY = rQBSP.m_Vertices[i].m_TexCoord[1][1];

		Colors[i].m_fX = rQBSP.m_Vertices[i].m_Color[0] / 255.f;
		Colors[i].m_fY = rQBSP.m_Vertices[i].m_Color[1] / 255.f;
		Colors[i].m_fZ = rQBSP.m_Vertices[i].m_Color[2] / 255.f;
	}

	if(!m_bInitializedBufferObjects) {
		glGenBuffers(7, m_BufferObjects);
	}
	// Positions
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[0]);
	glBufferData(	GL_ARRAY_BUFFER, (rQBSP.m_Vertices.size() * 3 * sizeof(GLfloat)),
					Positions.data(), GL_STATIC_DRAW);

	// TexCoords
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[1]);
	glBufferData(	GL_ARRAY_BUFFER, (rQBSP.m_Vertices.size() * 2 * sizeof(GLfloat)),
					TexCoords.data(), GL_STATIC_DRAW);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[2]);
	glBufferData(	GL_ARRAY_BUFFER, (rQBSP.m_Vertices.size() * 3 * sizeof(GLfloat)),
					Normals.data(), GL_STATIC_DRAW);

	// Lightmap
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[3]);
	glBufferData(	GL_ARRAY_BUFFER, (rQBSP.m_Vertices.size() * 2 * sizeof(GLfloat)),
					Lightmaps.data(), GL_STATIC_DRAW);

	// Color
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[4]);
	glBufferData(	GL_ARRAY_BUFFER, (rQBSP.m_Vertices.size() * 3 * sizeof(GLfloat)),
					Colors.data(), GL_STATIC_DRAW);

	if(!m_bInitializedBufferObjects) {
		this->m_uiVAO = 0;
		glGenVertexArrays(1, &this->m_uiVAO);
		glBindVertexArray(this->m_uiVAO);
		// Positions
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
		// TexCoords
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(1);
		// Normals
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(2);
		// Lightmap
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[3]);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(3);
		// Color
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[4]);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(4);
	}

	//
	//
	//
	std::vector<TPatchData> PatchBufferData;
	std::vector<float> VertexBufferData;
	size_t c = 0;
	for(size_t i = 0; i < rQBSP.m_Faces.size(); ++i)
	{
		if(rQBSP.m_Faces[i].m_iType == 2) // Patches
		{
			int iPatchWidth = (rQBSP.m_Faces[i].m_Size[0]-1)/2;
			int iPatchHeight = (rQBSP.m_Faces[i].m_Size[1]-1)/2;
			int iPatchCount = iPatchWidth * iPatchHeight;
			for(int n = 0; n < iPatchWidth; n++)
			{
				for(int m = 0; m < iPatchHeight; m++)
				{
					VertexBufferData.push_back(0);
					PatchBufferData.push_back(TPatchData{});
					//
					memset(&PatchBufferData[c], 0, sizeof(TPatchData));
					VertexBufferData[c] = (float)c;
					for(int u = 0; u < 3; u++)
					{
						for(int v = 0; v < 3; v++)
						{
							int Index = ((((m*2)+u) * rQBSP.m_Faces[i].m_Size[0]) + ((n*2)+v)) + rQBSP.m_Faces[i].m_iVertex;

							//
							PatchBufferData[c].color[u][v][0] = rQBSP.m_Vertices[Index].m_Color[0]/255.f;
							PatchBufferData[c].color[u][v][1] = rQBSP.m_Vertices[Index].m_Color[1]/255.f;
							PatchBufferData[c].color[u][v][2] = rQBSP.m_Vertices[Index].m_Color[2]/255.f;
							PatchBufferData[c].color[u][v][3] = rQBSP.m_Vertices[Index].m_Color[3]/255.f;
							
							PatchBufferData[c].lightmap[u][v][0] = rQBSP.m_Vertices[Index].m_TexCoord[1][0];
							PatchBufferData[c].lightmap[u][v][1] = rQBSP.m_Vertices[Index].m_TexCoord[1][1];
							
							PatchBufferData[c].normal[u][v][0] = rQBSP.m_Vertices[Index].m_Normal[0];
							PatchBufferData[c].normal[u][v][1] = rQBSP.m_Vertices[Index].m_Normal[2];
							PatchBufferData[c].normal[u][v][2] = -rQBSP.m_Vertices[Index].m_Normal[1];

							PatchBufferData[c].position[u][v][0] = rQBSP.m_Vertices[Index].m_Position[0];
							PatchBufferData[c].position[u][v][1] = rQBSP.m_Vertices[Index].m_Position[2];
							PatchBufferData[c].position[u][v][2] = -rQBSP.m_Vertices[Index].m_Position[1];

							PatchBufferData[c].texcoord[u][v][0] = rQBSP.m_Vertices[Index].m_TexCoord[0][0];
							PatchBufferData[c].texcoord[u][v][1] = rQBSP.m_Vertices[Index].m_TexCoord[0][1];
						}
					}
					c++;
				}
			}
		}
	}
	int32_t iPatchesCounted = 0;
	for(size_t i = 0; i < rQBSP.m_Faces.size(); i++)
	{
		if(rQBSP.m_Faces[i].m_iType == 2)
		{
			int iPatchWidth = (rQBSP.m_Faces[i].m_Size[0]-1)/2;
			int iPatchHeight = (rQBSP.m_Faces[i].m_Size[1]-1)/2;
			int iPatchCount = iPatchWidth * iPatchHeight;
			rQBSP.m_Faces[i].m_iMeshVertex = iPatchesCounted;
			iPatchesCounted += iPatchCount;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[5]);
	glBufferData(	GL_ARRAY_BUFFER, (VertexBufferData.size() * sizeof(GLint)),
					VertexBufferData.data(), GL_STATIC_DRAW);
	if(!m_bInitializedBufferObjects) {
		this->m_uiPatchVAO = 0;
		glGenVertexArrays(1, &this->m_uiPatchVAO);
		glBindVertexArray(this->m_uiPatchVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferObjects[5]);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(0);
	}

	//
	//
	//
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_BufferObjects[6]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(TPatchData)*PatchBufferData.size(), PatchBufferData.data(), GL_STATIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	this->m_uiPatchBufferSSBO = m_BufferObjects[6];

	//

	GLuint block_index = 0;
	block_index = glGetProgramResourceIndex(CEngineCore::GetInstance().m_pRenderer->m_Shaders.find("patch")->second.m_uiShaderProgram, GL_SHADER_STORAGE_BLOCK, "patch_data");

	GLuint ssbo_binding_point_index = 0;
	glShaderStorageBlockBinding(CEngineCore::GetInstance().m_pRenderer->m_Shaders.find("patch")->second.m_uiShaderProgram, block_index, ssbo_binding_point_index);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, m_BufferObjects[6]);

	//

	this->m_LightmapTextures.resize(rQBSP.m_LightMaps.size());
	glGenTextures((GLsizei)this->m_LightmapTextures.size(), this->m_LightmapTextures.data());
	for(size_t i = 0; i < this->m_LightmapTextures.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->m_LightmapTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, &rQBSP.m_LightMaps[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	//
	m_bInitializedBufferObjects = true;
}

int32_t FindLeaf(const TVector3f& _krPoint)
{
	const TQBSP& krQBSP = CEngineCore::GetInstance().m_pClient->m_BSP;
	int32_t iIndex = 0;

	for(; iIndex >= 0;)
	{
		const TFileNode& krNode = krQBSP.m_Nodes[iIndex];
		const TFilePlane& krPlane = krQBSP.m_Planes[krNode.m_iPlane];

		const TVector3f kNormal{krPlane.m_Normal[0], krPlane.m_Normal[2], -krPlane.m_Normal[1]};
		
		const float kfDistance = DotProduct(_krPoint, kNormal) - krPlane.m_fDistance;

		if(kfDistance > 0.f)
		{
			iIndex = krNode.m_Children[0];
		}
		else
		{
			iIndex = krNode.m_Children[1];
		}
	}

	return((-iIndex)-1);
}

bool IsVisible(const int32_t _kiVisCluster, const int32_t _kiTestCluster)
{
	const TQBSP& krQBSP = CEngineCore::GetInstance().m_pClient->m_BSP;
	if(_kiVisCluster < 0)
	{
		return(true);
	}
	int32_t i = (_kiVisCluster * krQBSP.m_VisData.m_iSizeVecs) + (_kiTestCluster / 8);
	const uint8_t kucVisSet = krQBSP.m_VisData.m_Vecs[i];

	return (kucVisSet & (1 << (_kiTestCluster % 8))) != 0;
}

bool IsVisibleFromFrustum(const int32_t _kiLeaf)
{
	CClient* pClient = CEngineCore::GetInstance().m_pClient;
	const TQBSP& krQBSP = pClient->m_BSP;
	//
	TVector3f Min{(float)krQBSP.m_Leafs[_kiLeaf].m_Mins[0], (float)krQBSP.m_Leafs[_kiLeaf].m_Mins[2], (float)-krQBSP.m_Leafs[_kiLeaf].m_Mins[1]};
	TVector3f Max{(float)krQBSP.m_Leafs[_kiLeaf].m_Maxs[0], (float)krQBSP.m_Leafs[_kiLeaf].m_Maxs[2], (float)-krQBSP.m_Leafs[_kiLeaf].m_Maxs[1]};
	TVector3f Mid = ScalarMultiply(TVector3f{}, Add(TVector3f{}, Min, Max), 1.0f/2.0f);
	float fDiameter = VectorMagnitude(Subtract(TVector3f{}, Max, Min));
	//
	const float kfAspectRatio = (float)CEngineCore::GetInstance().m_pWindow->m_iWidth / (float)CEngineCore::GetInstance().m_pWindow->m_iHeight;

	TVector3f Forward = QuaternionRotate(TVector3f(), TVector3f{0.0f, 0.0f, 1.0f}, pClient->m_CameraOrientation);

	TVector3f Bottom = QuaternionRotate(TVector3f(), Normalize(TVector3f{}, TVector3f{0.0f, 1.0f, 1.0f}), pClient->m_CameraOrientation);
	TVector3f Top = QuaternionRotate(TVector3f(), Normalize(TVector3f{}, TVector3f{0.0f, -1.0f, 1.0f}), pClient->m_CameraOrientation);

	TVector3f Right = QuaternionRotate(TVector3f(), Normalize(TVector3f{}, TVector3f{1.0f, 0.0f, kfAspectRatio}), pClient->m_CameraOrientation);
	TVector3f Left = QuaternionRotate(TVector3f(), Normalize(TVector3f{}, TVector3f{-1.0f, 0.0f, kfAspectRatio}), pClient->m_CameraOrientation);

	TVector3f Planes[5] = { Forward, Bottom, Top, Left };

	for(size_t i = 0; i < 5; i++)
	{
		float fDistance = DotProduct(Planes[i], pClient->m_CameraPosition);

		float fPointDP = DotProduct(Mid, Planes[i]);
		if((fPointDP - fDiameter) > fDistance)
		{
			return(false);
		}
	}

	TVector3f Points[8] ={	TVector3f{Min.m_fX, Min.m_fY, Min.m_fZ},
							TVector3f{Min.m_fX, Min.m_fY, Max.m_fZ},
							TVector3f{Min.m_fX, Max.m_fY, Min.m_fZ},
							TVector3f{Min.m_fX, Max.m_fY, Max.m_fZ},
							TVector3f{Max.m_fX, Min.m_fY, Min.m_fZ},
							TVector3f{Max.m_fX, Min.m_fY, Max.m_fZ},
							TVector3f{Max.m_fX, Max.m_fY, Min.m_fZ},
							TVector3f{Max.m_fX, Max.m_fY, Max.m_fZ} };

	for(size_t i = 0; i < 5; i++)
	{
		float fDistance = DotProduct(Planes[i], pClient->m_CameraPosition);
		int iInCount = 8;
		for(size_t j = 0; j < 8; j++)
		{
			float fPointDP = DotProduct(Points[j], Planes[i]);
			if(fPointDP > fDistance)
			{
				iInCount--;
			}
		}
		if(iInCount == 0)
		{
			return(false);
		}
	}

	return(true);
}

void CRenderer::FindVisibleLeafs(const int32_t _kiCurrentLeaf)
{
	const TQBSP& krQBSP = CEngineCore::GetInstance().m_pClient->m_BSP;
	for(int32_t i = 0; i < krQBSP.m_Leafs.size(); ++i)
	{
		if(IsVisible(krQBSP.m_Leafs[_kiCurrentLeaf].m_iCluster, krQBSP.m_Leafs[i].m_iCluster))
		{
			const size_t kByte = i/8;
			const size_t kBit = ((size_t)0x01 << (size_t)(i%8));
			this->m_LeafVisibility[kByte] |= kBit;
		}
	}
}

void CRenderer::GetLeafFaces(const int32_t _kiLeaf)
{
	const TQBSP& krQBSP = CEngineCore::GetInstance().m_pClient->m_BSP;

	for(int32_t i = 0; i < krQBSP.m_Leafs[_kiLeaf].m_iNumLeafFaces; i++)
	{
		const int32_t f = krQBSP.m_LeafFaces[krQBSP.m_Leafs[_kiLeaf].m_iLeafFace + i].m_iFace;
		const size_t kByte = f/8;
		const size_t kBit = (size_t)((size_t)0x01 << (size_t)(f%8));
		if(!(this->m_FaceVisibility[kByte] & kBit))
		{
			this->m_FaceVisibility[kByte] |= kBit;
			this->m_VisibleFaces[this->m_iVisibleFaceCount] = f;
			this->m_iVisibleFaceCount++;
		}
	}

	int q = 7;
}