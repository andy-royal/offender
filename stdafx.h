// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef _WINDOWS
#pragma once

#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"
#include "mydbgnew.h"

#include "targetver.h"

// LEAN_AND_MEAN breaks GDI+
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#include <iostream>
#include <setjmp.h>
#include <stdio.h>
#define _USE_MATH_DEFINES // M_PI etc.
#include <math.h>
#include <stdint.h>
//#define GDIPVER 0x0110    // Default is 0x0100 if GDIPVER undefined, need 0x0110 for ImageItemData
//#include <GdiPlus.h>
#include <olectl.h>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <boost/crc.hpp>
#include <time.h>

#define GLEW_STATIC
#include "gl\glew.h"
#include "gl\wglew.h"

#include "opengl_utils.h"
#include "object.h"
#include "mesh.h"
#include "rendergroup.h"
#include "camera.h"
#include "terrain.h"
#include "world.h"
#include "meshes/alienship.h"
#include "player.h"
#endif
