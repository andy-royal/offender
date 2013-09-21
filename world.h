#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
#include <list>
#include <vector>
#include "opengl_utils.h"
#include "object.h"
#endif

using namespace std;
using namespace OpenGLUtils;

#define STANDARD_GRAVITY (9.81f / (60.0f * 60.0f * 20.0f))

namespace Offender {

    class Object; // Forward declaration
    
    class World {
        private:
            DisplayContext*          m_display_context;
            list<auto_ptr<Object>>   m_objects;
            ObjMat                   m_ViewPersp;
            ObjMat                   m_View;
            list<auto_ptr<Terrain>>  m_Terrain;
            void                     CalcViewPersp();
            Camera                   m_camera;
            Object*                  m_camera_target;
            OBJ_NUMTYPE              m_camera_chase_distance;
            void                     MoveCamera();
        public:
            World(DisplayContext*);
            DisplayContext* GetDisplayContext() { return m_display_context; };
            ObjMat          GetView() { return m_View; };
            ObjMat          GetViewPersp() { return m_ViewPersp; };
            GLboolean       AddObject(Object*);
            GLboolean       CheckForCollisions();
            GLboolean       MoveAll();
            GLboolean       DrawAll();
            GLboolean       Iteration();
            void            SetCameraTarget(Object* l_target) { m_camera_target = l_target; };
            void            EndOfTheWorld() {};
            ObjVec          GetGravity(Object* l_object);
    };

} // namespace Offender
