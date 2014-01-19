#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
//#include "opengl_utils.h"
#include "object.h"
#endif

//using namespace std;
//using namespace OpenGLUtils;

#define ROTATION_SPEED 0.25f;

namespace Offender {

    class Player : public Object {
        private:
            GLfloat         m_rotation;
            GLfloat         m_tilt;
        public:
            Player(World*, RenderGroup*, Mesh*, ObjPos, ObjVec, ObjQuat);
            void            CollisionHandler();
            GLboolean       Move();
    };

}
