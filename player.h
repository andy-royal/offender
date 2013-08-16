#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
//#include "opengl_utils.h"
#include "object.h"
#endif

//using namespace std;
//using namespace OpenGLUtils;

namespace Offender {

    class Player : public AlienShip {
        private:
            GLuint          m_vertexBuffer;
            GLuint          m_indexBuffer;
        public:
            Player(World*, ObjPos, ObjVec, ObjQuat);
            void            CollisionHandler();
            GLboolean       Move();
    };

}
