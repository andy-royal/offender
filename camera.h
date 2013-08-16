#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
//#include "opengl_utils.h"
#include "object.h"
#endif

using namespace std;
//using namespace OpenGLUtils;

namespace Offender {

    class Camera : public Object {
        public:
            Camera() : Object(NULL, ObjPos(0.0f, 0.0f, 0.0f),
                                    ObjVec(0.0f, 0.0f, 0.0f),
                                    ObjQuat(cos(M_PI/6), sin(M_PI/6), 0.0f, 0.0f))
                       { SetModelMatrix(); };
            Camera(World* l_world) : Object(l_world, ObjPos(0.0f, 50.0f, 1900.0f),
                                                     ObjVec(0.0f, 0.0f, 0.0f),
                                                     ObjQuat(cos(static_cast<OBJ_NUMTYPE>(-M_PI)/100.0f), sin(static_cast<OBJ_NUMTYPE>(-M_PI)/100.0f), 0.0f, 0.0f))
                                     { SetModelMatrix(); };
            GLboolean       Move() { return GL_TRUE; };
            GLboolean       Draw() { return GL_TRUE; };
    };

} // namespace Offender
