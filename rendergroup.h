#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
#include <list>
//#include <vector>
//#include "opengl_utils.h"
//#include "object.h"
#endif

namespace Offender {

    class RenderGroup {
        private:
            GLuint                  m_programObject;
            GLuint                  m_vertexBuffer;
            GLuint                  m_indexBuffer;
            GLuint                  m_vao;
            list<auto_ptr<Mesh>>    m_meshes;
            list<Object*>           m_objects;
            GLuint                  m_mv;
            GLuint                  m_mvp;
            GLuint                  m_colour;
            GLuint                  m_lightvector;
            GLuint                  m_Ka;
            GLuint                  m_La;
            GLuint                  m_Kd;
            GLuint                  m_Ld;
            GLuint                  m_Ks;
            GLuint                  m_Ls;
            GLuint                  m_Shininess;
        public:
            GLboolean               SetupBuffers();
            GLboolean               Draw();
            void                    AddMesh(Mesh* l_mesh) { m_meshes.push_back(auto_ptr<Mesh>(l_mesh)); };
            void                    AddObject(Object* l_obj) { m_objects.push_back(l_obj); };
            GLuint                  GetModelViewHandle() { return m_mv; };
            GLuint                  GetMVPHandle() { return m_mvp; };
            GLuint                  GetColourHandle() { return m_colour; };
            GLuint                  GetLightVectorHandle() { return m_lightvector; };
            GLuint                  GetKaHandle() { return m_Ka; };
            GLuint                  GetLaHandle() { return m_La; };
            GLuint                  GetKdHandle() { return m_Kd; };
            GLuint                  GetLdHandle() { return m_Ld; };
            GLuint                  GetKsHandle() { return m_Ks; };
            GLuint                  GetLsHandle() { return m_Ls; };
            GLuint                  GetShininessHandle() { return m_Shininess; };
    };

} // namespace Offender
