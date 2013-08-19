#include "stdafx.h"

#ifndef _WINDOWS
#include "object.h"
#endif

using namespace std;
using namespace OpenGLUtils;

namespace Offender {

    Object::Object(World* l_world, ObjPos l_pos, ObjVec l_vec, ObjQuat l_orient) :
                   m_Position(l_pos), m_Velocity(l_vec), m_Orientation(l_orient) {
        m_world = l_world;
        m_DeathThroes = GL_FALSE;
    }

//#version 400
//                   
//layout (location = 0) in vec3 VertexPosition;
//layout (location = 1) in vec3 VertexNormal;
//
//out vec3 LightIntensity;
//
//struct LightInfo {
//  vec4 Position; // Light position in eye coords.
//  vec3 La; // Ambient light intensity
//  vec3 Ld; // Diffuse light intensity
//  vec3 Ls; // Specular light intensity
//};
//uniform LightInfo Light;
//
//struct MaterialInfo {
//  vec3 Ka; // Ambient reflectivity
//  vec3 Kd; // Diffuse reflectivity
//  vec3 Ks; // Specular reflectivity
//  float Shininess; // Specular shininess factor
//};
//uniform MaterialInfo Material;
//
//uniform mat4 ModelViewMatrix;
//uniform mat3 NormalMatrix;
//uniform mat4 ProjectionMatrix;
//uniform mat4 MVP;
//
//void main()
//{
//  vec3 tnorm = normalize( NormalMatrix * VertexNormal);
//  vec4 eyeCoords = ModelViewMatrix * vec4(VertexPosition,1.0);
//  vec3 s = normalize(vec3(Light.Position - eyeCoords));
//  vec3 v = normalize(-eyeCoords.xyz);
//  vec3 r = reflect( -s, tnorm );
//  vec3 ambient = Light.La * Material.Ka;
//  float sDotN = max( dot(s,tnorm), 0.0 );
//  vec3 diffuse = Light.Ld * Material.Kd * sDotN;
//  vec3 spec = vec3(0.0);
//
//  if( sDotN > 0.0 )
//    spec = Light.Ls * Material.Ks * pow( max( dot(r,v), 0.0 ), Material.Shininess );
//
//  LightIntensity = ambient + diffuse + spec;
//  gl_Position = MVP * vec4(VertexPosition,1.0);
//}

    GLuint Object::DefaultProgram() {
        const string vShaderStr =
            "#version 150                                       \n"
            "                                                   \n"
            "uniform mat4  u_ModelViewMatrix;                   \n"
            "uniform mat4  u_MVPMatrix;                         \n"
            "uniform vec4  u_colour;                            \n"
            "uniform vec3  u_LightVector;                       \n"
            "uniform vec3  Ka; // Ambient reflectivity          \n"
            "uniform vec3  La;                                  \n"
            "uniform vec3  Kd; // Diffuse reflectivity          \n"
            "uniform vec3  Ld;                                  \n"
            "uniform vec3  Ks; // Specular reflectivity         \n"
            "uniform vec3  Ls;                                  \n"
            "uniform float Shininess;                           \n"
            "                                                   \n"
            "in vec3 a_Position;                                \n"
            "in vec3 a_Normal;                                  \n"
            "                                                   \n"
            "out vec4 v_colour;                                 \n"
            "                                                   \n"
            "void main()                                        \n"
            "{                                                  \n"
            "  gl_Position = u_MVPMatrix * vec4(a_Position, 1); \n"
            "  vec3 tnorm = vec3(normalize(u_ModelViewMatrix * vec4(a_Normal, 1.0)));\n" // Should be Normal Matrix
            "  vec4 eyeCoords = u_ModelViewMatrix * vec4(a_Position, 1.0);\n"
            "  vec3 v = normalize(-eyeCoords.xyz);              \n"
            "  vec3 r = reflect( -u_LightVector, tnorm );       \n"
            "  vec3 l_LightIntensity = La * Ka                  \n"
            "                        + Ld * Kd * max(dot(mat3(u_ModelViewMatrix) * a_Normal, u_LightVector), 0.0)\n"
            "                        + Ls * Ks * pow(max(dot(r,v), 0.0), Shininess);\n"
            "  v_colour = u_colour * vec4(l_LightIntensity, 1.0);  \n"
            "}                                                  \n";
        const string fShaderStr =  
            "#version 150                                       \n"
            "                                                   \n"
            "precision mediump float;                           \n"
            "                                                   \n"
            "in vec4 v_colour;                                  \n"
            "out vec4 colourout;                                \n"
            "                                                   \n"
            "void main()                                        \n"
            "{                                                  \n"
            "  colourout = v_colour;                            \n"
            "}                                                  \n";

        static ParamBindings AttrBindings = AssignAttrBindings();

        static GLuint programObject = LoadProgram(vShaderStr, fShaderStr, AttrBindings);

        return programObject;
    }

    ParamBindings Object::AssignAttrBindings() {
        ParamBindings AttrBindings;
        AttrBindings[POSITION_HANDLE] = "a_Position";
        AttrBindings[NORMAL_HANDLE] = "a_Normal";
        return AttrBindings;
    }

    GLuint Object::GetProgramObject() {
        static GLuint programObject = DefaultProgram();
        return programObject;
    }

    GLuint Object::GetModelViewHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_mvp = glGetUniformLocation(programObject, "u_ModelViewMatrix");
        return l_mvp;
    }

    GLuint Object::GetMVPHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_mvp = glGetUniformLocation(programObject, "u_MVPMatrix");
        return l_mvp;
    }

    GLuint Object::GetColourHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_colour = glGetUniformLocation(programObject, "u_colour");
        return l_colour;
    }

    GLuint Object::GetLightVectorHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_vector = glGetUniformLocation(programObject, "u_LightVector");
        return l_vector;
    }

    GLuint Object::GetKaHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Ka = glGetUniformLocation(programObject, "Ka");
        return l_Ka;
    }

    GLuint Object::GetLaHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_La = glGetUniformLocation(programObject, "La");
        return l_La;
    }

    GLuint Object::GetKdHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Kd = glGetUniformLocation(programObject, "Kd");
        return l_Kd;
    }

    GLuint Object::GetLdHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Ld = glGetUniformLocation(programObject, "Ld");
        return l_Ld;
    }

    GLuint Object::GetKsHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Ks = glGetUniformLocation(programObject, "Ks");
        return l_Ks;
    }

    GLuint Object::GetLsHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Ls = glGetUniformLocation(programObject, "Ls");
        return l_Ls;
    }
    GLuint Object::GetShininessHandle() {
        static GLuint programObject = DefaultProgram();
        static GLuint l_Shininess = glGetUniformLocation(programObject, "Shininess");
        return l_Shininess;
    }

    GLboolean Object::DrawSetup() {
        // Calculate colour and MVP matrix
        GLfloat l_colour[3] = {0.8f, 0.8f, 0.8f};
        ObjMat Model = *GetModelMatrix();
        ObjMat View = GetWorld()->GetView();
        ObjMat ViewPersp = GetWorld()->GetViewPersp();
        ObjMat mv = View * Model;
        ObjMat mvp = ViewPersp * Model;
        ObjVec l_LightSource = ObjVec(0.5f, sqrt(3.0f) / 2.0f, 0.0f);
        ObjVec l_LightEye = View * l_LightSource;

        // Define reflectivity constants
        const GLfloat Ka = 0.3f;
        const GLfloat Kd = 1.5f;
        const GLfloat Ks = 0.0f;

        // Get ready for some hot GLSL action
        glUseProgram (GetProgramObject());

        // Set uniforms for MVP/colour
        glUniformMatrix4fv(GetModelViewHandle(), 1, GL_TRUE, mv.data());
        glUniformMatrix4fv(GetMVPHandle(), 1, GL_TRUE, mvp.data());
        glUniform4fv(GetColourHandle(), 1, l_colour);
        glUniform3f(GetLightVectorHandle(), l_LightEye[0], l_LightEye[1], l_LightEye[2]);
        glUniform3f(GetKaHandle(), Ka, Ka, Ka);
        glUniform3f(GetLaHandle(), 1.0f, 1.0f, 1.0f);
        glUniform3f(GetKdHandle(), Kd, Kd, Kd);
        glUniform3f(GetLdHandle(), 1.0f, 1.0f, 1.0f);
        glUniform3f(GetKsHandle(), Ks, Ks, Ks);
        glUniform3f(GetLsHandle(), 1.0f, 1.0f, 1.0f);
        glUniform1f(GetShininessHandle(), 1.0f);

        return GL_TRUE;
    }

} // namespace Offender
