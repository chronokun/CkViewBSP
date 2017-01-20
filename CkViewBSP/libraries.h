//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

#pragma once

#ifndef __LIBRARIES_H__
#define __LIBRARIES_H__

// Visual Leak Detector
#if defined(DEBUG) || defined(_DEBUG)  
//#include <vld.h>
#endif

// Platform Libraries
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Chronokun Libraries
#include "ckmath/ckmath.h"

// C++ Libraries
#include <cstdint>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <fstream>
#include <chrono>

// OpenGL
#include <GL/gl.h>
#include "include/glext.h"
#include "include/wglext.h"
#include "glfuncs.h"

// NanoVG
#include "nanovg/nanovg.h"

#endif