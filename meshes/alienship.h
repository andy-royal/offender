#pragma once
#include "stdafx.h"

namespace Offender {

    class AlienShip : public Object {
        private:
            GLuint          m_vertexBuffer;
            GLuint          m_indexBuffer;
            GLuint          m_vao;
            void            InitGeometry();
        public:
            AlienShip(World*, ObjPos, ObjVec, ObjQuat);
            GLboolean       Draw();
    };

} // namespace Offender
