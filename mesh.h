#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
//#include <list>
//#include <vector>
//#include "opengl_utils.h"
//#include "object.h"
#endif

namespace Offender {

    class Mesh {
        protected:
            int                     m_vertexoffset;
            int                     m_indexoffset;
            vector<ObjectVertex>    m_vertices;
            vector<GLuint>          m_indices;
            vector<GLuint>          m_faces;
            OBJ_NUMTYPE             m_CollisionRadius;
        public:
            Mesh(const char* filename);
            int                 GetVertexCount() { return m_vertices.size(); };
            int                 GetIndexCount() { return m_indices.size(); };
            void                SetVertexOffset(int val) { m_vertexoffset = val; };
            void                SetIndexOffset(int val) { m_indexoffset = val; };
            ObjectVertex        GetVertex(int n) { return m_vertices[n]; };
            int                 GetIndex(int n) { return m_indices[n]; };
            OBJ_NUMTYPE         GetCollisionRadius() { return m_CollisionRadius; };
            void                SetCollisionRadius(OBJ_NUMTYPE l_radius) { m_CollisionRadius = l_radius; };
            GLboolean           Draw();
    };

} // namespace Offender
