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

    class Player : public AlienShip {
        private:
            GLuint          m_vertexBuffer;
            GLuint          m_indexBuffer;
            GLfloat         m_rotation;
            GLfloat         m_tilt;
        public:
            Player(World*, ObjPos, ObjVec, ObjQuat);
            void            CollisionHandler();
            GLboolean       Move();
    };

}
