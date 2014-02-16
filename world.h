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
            RenderGroup              m_render;
            list<auto_ptr<Object>>   m_objects;
            ObjMat                   m_ViewPersp;
            ObjMat                   m_View;
            list<auto_ptr<Terrain>>  m_Terrain;
            void                     CalcViewPersp();
            Camera                   m_camera;
            Object*                  m_camera_target;
            OBJ_NUMTYPE              m_camera_chase_distance;
            void                     MoveCamera();

            // Factory
            // CreateFunc can't be static if we need to use member functions or this. Hence PCreateFunc must be a
            // pointer to member function, as pointers to global functions are only compatible with static member
            // functions.
            template <typename T> Object* CreateFunc(ObjPos l_position, ObjVec l_velocity, ObjQuat l_orientation) {
                return new T(this, &m_render, l_position, l_velocity, l_orientation);
            }
            typedef Object* (World::*PCreateFunc)(ObjPos, ObjVec, ObjQuat);
            map<string,PCreateFunc>  m_CreateFuncs;    // This requires #include <string>, and gives really weird errors without it

        public:
            World(DisplayContext*);
            DisplayContext* GetDisplayContext() { return m_display_context; };
            RenderGroup*    GetRendergroup() { return &m_render; };
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

            // Factory continued. Again note pointer to member function.
            template <typename T> void Register(const char* name) {
                // Register create function
                m_CreateFuncs[name] = &World::CreateFunc<T>;

                // Load meshes
                vector<string> l_mesh_names = T::GetMeshNames();
                vector<string>::iterator it;
                vector<Mesh*> l_meshes;
                for (it=l_mesh_names.begin(); it!=l_mesh_names.end(); it++) {
                    Mesh* l_mesh = new Mesh(it->data());
                    m_render.AddMesh(l_mesh);
                    l_meshes.push_back(l_mesh);
                }
                T::SetMeshes(l_meshes);
            }
            Object* GetInstance(string l_name, ObjPos l_position, ObjVec l_velocity, ObjQuat l_orientation) {
                Object* tmp = (this->*m_CreateFuncs[l_name])(l_position, l_velocity, l_orientation);
                AddObject(tmp);
                m_render.AddObject(tmp);
                return tmp;
            }
    };

} // namespace Offender
