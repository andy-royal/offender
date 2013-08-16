#include "stdafx.h"

#ifndef _WINDOWS
#include "world.h"
#include "object.h"
#endif

namespace Offender {

    World::World(DisplayContext* l_context) {
        m_display_context = l_context;
//        m_ViewPersp = m_display_context->GetPerspective();
        m_View.LoadIdentity();
        m_camera = Camera(this);
        m_camera_target = NULL;
        m_camera_chase_distance = 4.0f;
        m_ViewPersp = m_display_context->GetPerspective() * m_camera.GetModelMatrix()->Inverse();

#ifdef WIREFRAME
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);               // Black Background
#else
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);               // Black Background
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);               // Sky blue Background
#endif

        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f, -1920.0f, -1920.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,  -640.0f, -1920.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,   640.0f, -1920.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f, -1920.0f,  -640.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,  -640.0f,  -640.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,   640.0f,  -640.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f, -1920.0f,   640.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,  -640.0f,   640.0f)));
        m_Terrain.push_back(auto_ptr<Terrain>(new Terrain(&m_ViewPersp, 128, 128, 10.0f, 10.0f,   640.0f,   640.0f)));
#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_Terrain.front().get()->ReportTimes();
#endif
    }

    void World::CalcViewPersp() {
        m_ViewPersp = m_display_context->GetPerspective() * m_View;
    }

    GLboolean World::AddObject(Object* l_object) {
        m_objects.push_back(auto_ptr<Object>(l_object));
        return GL_TRUE;
    }

    GLboolean World::CheckForCollisions() {
        list<auto_ptr<Object>>::iterator it;
        for (it = m_objects.begin(); it != m_objects.end(); it++) {
            // Tile collision
            list<auto_ptr<Terrain>>::iterator tit;
            for (tit = m_Terrain.begin(); tit != m_Terrain.end(); tit++) {
                if (tit->get()->CheckForCollision(it->get())) {
                    it->get()->CollisionHandler();
                }
            }
            //if (m_Terrain.CheckForCollision(it->get())) {
            //    it->get()->CollisionHandler();
            //}
        }

        return GL_TRUE;
    }

    GLboolean World::MoveAll() {
        list<auto_ptr<Object>>::iterator it;
        for (it = m_objects.begin(); it != m_objects.end(); it++) {
            if (!it->get()->Move()) {
                return GL_FALSE;
            }
        }

        MoveCamera();

        return GL_TRUE;
    }

    GLboolean World::DrawAll() {
        // Set the viewport
        glViewport (0, 0, m_display_context->GetWidth(), m_display_context->GetHeight());

        // Clear the color and depth buffers
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw terrain
        list<auto_ptr<Terrain>>::iterator tit;
        for (tit = m_Terrain.begin(); tit != m_Terrain.end(); tit++) {
            if (!tit->get()->Draw()) {
                return GL_FALSE;
            }
        }

        // Draw each object
        list<auto_ptr<Object>>::iterator it;
        for (it = m_objects.begin(); it != m_objects.end(); it++) {
            if (!it->get()->Draw()) {
                return GL_FALSE;
            }
        }

        return GL_TRUE;
    }

    GLboolean World::Iteration() {
        DrawAll();
        MoveAll();
        CheckForCollisions();

        return GL_TRUE;
    }

    void World::MoveCamera() {
        if (m_camera_target) {
            ObjQuat l_orientation = m_camera_target->GetOrientation();
            ObjVec l_direction(0.0f, 0.0f, -1.0f);
            if (m_camera_target->GetDeathThroes()) {
                static OBJ_NUMTYPE angle = 0.0f;
                angle += static_cast<OBJ_NUMTYPE>(1.0f);
                Vector<OBJ_NUMTYPE> rot_axis(0.0f, 1.0f, 0.0f);
                AngleAndAxis<OBJ_NUMTYPE> rotation(angle, rot_axis);
                l_orientation = l_orientation * rotation.ToQuaternion();
            }
            l_direction = l_direction * l_orientation;
            m_camera.SetOrientation(l_orientation);
            m_camera.SetPosition(m_camera_target->GetPosition() - m_camera_chase_distance * l_direction);
            m_camera.SetModelMatrix();
            m_View = m_camera.GetModelMatrix()->Inverse();
            CalcViewPersp();
        } else {
            //MessageBox(NULL,L"Camera Target is NULL.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);            
        }
    }

} // namespace Offender
