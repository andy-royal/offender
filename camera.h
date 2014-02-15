#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
//#include "opengl_utils.h"
#include "object.h"
#endif

using namespace std;
//using namespace OpenGLUtils;

namespace Offender {

    class Camera {
        private:
            ObjMat                  m_ModelMatrix;
            ObjPos                  m_Position;
            ObjQuat                 m_Orientation;
        public:
            Camera();
            ObjMat*                 GetModelMatrix() { return &m_ModelMatrix; };
            void                    SetModelMatrix() { m_ModelMatrix = ModelMatrix(m_Position, m_Orientation); };
            void                    SetPosition(ObjPos l_newpos) { m_Position = l_newpos; };
            void                    SetOrientation(ObjQuat l_Orientation) { m_Orientation = l_Orientation; };
    };

} // namespace Offender
