#pragma once
#include "stdafx.h"

using namespace std;

namespace Offender {

    GLboolean RenderGroup::SetupBuffers() {
        const string vShaderStr =
            "#version 140                                       \n"
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
            "#version 140                                       \n"
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

        ParamBindings AttrBindings;
        AttrBindings[POSITION_HANDLE] = "a_Position";
        AttrBindings[NORMAL_HANDLE] = "a_Normal";

        m_programObject = LoadProgram(vShaderStr, fShaderStr, AttrBindings);

        m_mv = glGetUniformLocation(m_programObject, "u_ModelViewMatrix");
        m_mvp = glGetUniformLocation(m_programObject, "u_MVPMatrix");
        m_colour = glGetUniformLocation(m_programObject, "u_colour");
        m_lightvector = glGetUniformLocation(m_programObject, "u_LightVector");
        m_Ka = glGetUniformLocation(m_programObject, "Ka");
        m_La = glGetUniformLocation(m_programObject, "La");
        m_Kd = glGetUniformLocation(m_programObject, "Kd");
        m_Ld = glGetUniformLocation(m_programObject, "Ld");
        m_Ks = glGetUniformLocation(m_programObject, "Ks");
        m_Ls = glGetUniformLocation(m_programObject, "Ls");
        m_Shininess = glGetUniformLocation(m_programObject, "Shininess");

        ObjectVertex* vertices;
        GLuint* indices;

        int vertex_count = 0;
        int index_count = 0;

        list<auto_ptr<Mesh>>::iterator it;
        for (it = m_meshes.begin(); it != m_meshes.end(); it++) {
            vertex_count += it->get()->GetVertexCount();
            index_count  += it->get()->GetIndexCount();
        }

        vertices = new ObjectVertex[vertex_count];
        indices = new GLuint[index_count];

        int vertex_ptr = 0;
        int index_ptr = 0;

        for (it = m_meshes.begin(); it != m_meshes.end(); it++) {
            Mesh* tmp = it->get();
            tmp->SetVertexOffset(vertex_ptr);
            tmp->SetIndexOffset(index_ptr);

            int vcount = tmp->GetVertexCount();
            int icount = tmp->GetIndexCount();

            // Copy vertices and indices into local array. N.B. As vertices may no longer start at offset zero, need to add offset of vertices to indices
            for (int vertex=0; vertex<vcount; vertex++) 
                vertices[vertex + vertex_ptr] = tmp->GetVertex(vertex);
            for (int index=0; index<icount; index++)
                indices[index + index_ptr] = tmp->GetIndex(index) + vertex_ptr;

            vertex_ptr += vcount;
            index_ptr += icount;
        }

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ObjectVertex) * vertex_count, vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &m_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * index_count, indices, GL_STATIC_DRAW);

        glVertexAttribPointer (POSITION_HANDLE, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), reinterpret_cast<void*>(offsetof(ObjectVertex,position)));
        glEnableVertexAttribArray (POSITION_HANDLE);
        glVertexAttribPointer (NORMAL_HANDLE, 3, GL_FLOAT, GL_FALSE, sizeof(ObjectVertex), reinterpret_cast<void*>(offsetof(ObjectVertex,normal)));
        glEnableVertexAttribArray (NORMAL_HANDLE);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        delete[] vertices;
        delete[] indices;

        CheckForOpenGLErrors();

        return GL_TRUE;
    }

    GLboolean RenderGroup::Draw() {


        // Select buffers
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);

        // Get ready for some hot GLSL action
        glUseProgram (m_programObject);

        list<Object*>::iterator it;
        for (it = m_objects.begin(); it != m_objects.end(); it++) {
            (*it)->Draw();
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        CheckForOpenGLErrors();

        return GL_TRUE;
    }

} // namespace Offender
