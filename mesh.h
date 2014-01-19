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
            int                 m_vertexcount;
            int                 m_indexcount;
            int                 m_vertexoffset;
            int                 m_indexoffset;
            ObjectVertex*       m_vertices;
            int*                m_indices;
            OBJ_NUMTYPE         m_CollisionRadius;
        public:
            int                 GetVertexCount() { return m_vertexcount; };
            int                 GetIndexCount() { return m_indexcount; };
            void                SetVertexOffset(int val) { m_vertexoffset = val; };
            void                SetIndexOffset(int val) { m_indexoffset = val; };
            ObjectVertex        GetVertex(int n) { return m_vertices[n]; };
            int                 GetIndex(int n) { return m_indices[n]; };
            OBJ_NUMTYPE         GetCollisionRadius() { return m_CollisionRadius; };
            void                SetCollisionRadius(OBJ_NUMTYPE l_radius) { m_CollisionRadius = l_radius; };
            virtual GLboolean   Draw()=0;
    };

} // namespace Offender
