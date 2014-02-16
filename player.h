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
            static Mesh*    m_mesh;
            static Mesh*    m_flame;
        public:
            Player(World*, RenderGroup*, ObjPos, ObjVec, ObjQuat);
            void            CollisionHandler();
            OBJ_NUMTYPE     GetCollisionRadius();
            GLboolean       Move();
            GLboolean       DrawMeshes();
            static vector<string>   GetMeshNames();
            static void             SetMeshes(vector<Mesh*>);
    };

}
