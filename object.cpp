#include "stdafx.h"

#ifndef _WINDOWS
#include "object.h"
#endif

using namespace std;
using namespace OpenGLUtils;

namespace Offender {

    Object::Object(World* l_world, RenderGroup* l_render, Mesh* l_mesh, ObjPos l_pos, ObjVec l_vec, ObjQuat l_orient) :
                   m_Position(l_pos), m_Velocity(l_vec), m_Orientation(l_orient) {
        m_world = l_world;
        m_rendergroup = l_render;
        m_mesh = l_mesh;
        m_DeathThroes = GL_FALSE;
    }

    OBJ_NUMTYPE Object::GetCollisionRadius() {
        return m_mesh->GetCollisionRadius();
    }

    GLboolean Object::Draw() {
        // Calculate colour and MVP matrix
        GLfloat l_colour[3] = {0.8f, 0.8f, 0.8f};
        ObjMat Model = *GetModelMatrix();
        ObjMat View = m_world->GetView();
        ObjMat ViewPersp = m_world->GetViewPersp();
        ObjMat mv = View * Model;
        ObjMat mvp = ViewPersp * Model;
        ObjVec l_LightSource = ObjVec(0.5f, sqrt(3.0f) / 2.0f, 0.0f);
        ObjVec l_LightEye = View * l_LightSource;

        // Define reflectivity constants
        const GLfloat Ka = 0.3f;
        const GLfloat Kd = 1.5f;
        const GLfloat Ks = 0.0f;

        // Set uniforms for MVP/colour
        glUniformMatrix4fv(m_rendergroup->GetModelViewHandle(), 1, GL_TRUE, mv.data());
        glUniformMatrix4fv(m_rendergroup->GetMVPHandle(), 1, GL_TRUE, mvp.data());
        glUniform4fv(m_rendergroup->GetColourHandle(), 1, l_colour);
        glUniform3f(m_rendergroup->GetLightVectorHandle(), l_LightEye[0], l_LightEye[1], l_LightEye[2]);
        glUniform3f(m_rendergroup->GetKaHandle(), Ka, Ka, Ka);
        glUniform3f(m_rendergroup->GetLaHandle(), 1.0f, 1.0f, 1.0f);
        glUniform3f(m_rendergroup->GetKdHandle(), Kd, Kd, Kd);
        glUniform3f(m_rendergroup->GetLdHandle(), 1.0f, 1.0f, 1.0f);
        glUniform3f(m_rendergroup->GetKsHandle(), Ks, Ks, Ks);
        glUniform3f(m_rendergroup->GetLsHandle(), 1.0f, 1.0f, 1.0f);
        glUniform1f(m_rendergroup->GetShininessHandle(), 1.0f);

        m_mesh->Draw();

        return GL_TRUE;
    }

} // namespace Offender
