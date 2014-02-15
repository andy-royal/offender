#include "stdafx.h"

#ifndef _WINDOWS
#include "player.h"
#endif

namespace Offender {

    Player::Player(World* l_world, RenderGroup* l_render, ObjPos l_pos, ObjVec l_vec, ObjQuat l_orient) :
                   Object(l_world, l_render, l_pos, l_vec, l_orient)  {
        m_rotation = 0;
        m_tilt = 0;
    }

    Mesh* Player::m_mesh;

    void Player::CollisionHandler() {
        //MessageBox(NULL,L"Oh dear, you crashed.",L"SPLAT",MB_OK|MB_ICONEXCLAMATION);
        SetDeathThroes(GL_TRUE);
        GetWorld()->EndOfTheWorld();
    }

    GLboolean Player::Move() {

#ifdef SCREENSHOT
        SetModelMatrix();
#else
        if (!GetDeathThroes()) {
            MouseInfo* l_mouseinfo = GetWorld()->GetDisplayContext()->MouseInfo();
#if 0
            Vector<OBJ_NUMTYPE> rot_axis(static_cast<OBJ_NUMTYPE>(l_mouseinfo->GetYDelta()),0.0f,static_cast<OBJ_NUMTYPE>(-l_mouseinfo->GetXDelta()));
            // Multiply it by orientation
            rot_axis = rot_axis * GetOrientation();
            // No need to normalise as we'll do it later
            AngleAndAxis<OBJ_NUMTYPE> rotation(l_mouseinfo->GetMouseSpeed() * rot_axis.Magnitude(), rot_axis);
            // Apply rotation to saved rotation. N.B. New rotation on LEFT
            ModifyOrientation(rotation.ToQuaternion());

            // Set Velocity
            ObjVec l_direction(0.0f, 0.0f, -1.0f);
            l_direction = l_direction * GetOrientation();
            SetVelocity(0.5f * l_direction);
            //SetVelocity(5.0f * l_direction);
#else
            ObjVec acceleration = GetWorld()->GetGravity(this);
            m_rotation -= l_mouseinfo->GetXDelta() * ROTATION_SPEED;
            m_tilt += l_mouseinfo->GetYDelta() * ROTATION_SPEED;
            AngleAndAxis<OBJ_NUMTYPE> rotation_x(m_rotation, ObjVec(0.0f, 1.0f, 0.0f));
            AngleAndAxis<OBJ_NUMTYPE> rotation_y(m_tilt, ObjVec(1.0f, 0.0f, 0.0f));
            SetOrientation(rotation_x.ToQuaternion() * rotation_y.ToQuaternion());

            if (l_mouseinfo->GetLeftDown()) {
                ObjVec l_direction(0.0f, 1.0f, 0.0f);
                l_direction = l_direction * GetOrientation();
                acceleration = acceleration + (l_direction * STANDARD_GRAVITY * 4);
            }
            SetVelocity(GetVelocity() + acceleration);
#endif
            // Move
            SetPosition(GetPosition() + GetVelocity());

            // Reset amount mouse has moved
            l_mouseinfo->ResetMouseMove();

            SetModelMatrix();
        }
#endif

        return GL_TRUE;
    }

    GLboolean Player::DrawMeshes() {
        m_mesh->Draw();
        return GL_TRUE;
    }

    OBJ_NUMTYPE Player::GetCollisionRadius() {
        return m_mesh->GetCollisionRadius();
    }

}
