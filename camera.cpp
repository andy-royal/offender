#include "stdafx.h"

#ifndef _WINDOWS
#include "object.h"
#include "camera.h"
#endif

using namespace std;
using namespace OpenGLUtils;

namespace Offender {

    Camera::Camera() : m_Orientation(ObjQuat(cos(M_PI/6), sin(M_PI/6), 0.0f, 0.0f)) {
         m_Position = ObjPos(0.0f, 0.0f, 0.0f);
         SetModelMatrix();
    };

}

