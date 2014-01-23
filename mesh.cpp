#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
#include <fstream>
#endif

#define MESH_PATH "..\\..\\GitHub\\offender\\meshes\\"

using namespace std;

namespace Offender {

    Mesh::Mesh(const char* filename) {
        string file = string(MESH_PATH) + filename + ".obj";

        string line;
        ifstream myfile (file.data());

        if (myfile.is_open()) {
            int ptr;
            string token;
            vector<string> tokenised;
            vector<GLfloat> vertex_normals;
            vector<GLuint> normal_indices;

            while (getline(myfile, line)) {
                ptr = 0;
                // Redundant as always cleared at end of line if non-empty
                //token.clear();
                //tokenised.clear();

                for (unsigned int i = 0; i < line.length(); i++) {
                    if (line[i] == ' ' || line[i] == '\t' || line[i] == '#') {
                        if (!token.empty()) {
                            tokenised.push_back(token);
                            token.clear();
                        }
                        if (line[i] == '#') break;
                    } else {
                        token += line[i];
                    }
                }
                if (!token.empty()) {
                    tokenised.push_back(token);
                    token.clear();
                }
                if (!tokenised.empty()) {
                    if (tokenised[0].compare("v") == 0) {
                        ObjectVertex tmp;
                        for (unsigned int i=0; i<3; i++) {
                            tmp.position[i] = static_cast<GLfloat>(atof(tokenised[i+1].c_str()));
                        }
                        m_vertices.push_back(tmp);
                    } else if (tokenised[0].compare("vn") == 0) {
                        for (unsigned int i=0; i<3; i++) {
                            vertex_normals.push_back(static_cast<GLfloat>(atof(tokenised[i+1].c_str())));
                        }
                    } else if (tokenised[0].compare("f") == 0) {
                        m_faces.push_back(tokenised.size() - 1);  // Number of faces is number of tokens excluding "f"
                        for (GLuint i=1; i < tokenised.size(); i++) {
                            size_t slashes = tokenised[i].find("//");
                            // std::vector and OpenGL indices start at 0, Wavefront OBJ indices start at 1, so -1 to all indices applied here
                            m_indices.push_back(atoi(tokenised[i].substr(0, slashes).c_str()) - 1);
                            normal_indices.push_back(atoi(tokenised[i].substr(slashes+2,string::npos).c_str()) - 1);
                        }
                    } else if ((tokenised[0].compare("mtllib") == 0) ||
                        (tokenised[0].compare("o") == 0) ||
                        (tokenised[0].compare("usemtl") == 0) ||
                        (tokenised[0].compare("s") == 0)) {
                        // Do nothing
                    } else {
#ifdef _WINDOWS
                        MessageBox(NULL,TEXT("Unrecognised line in OBJ file"),TEXT("Wavefront OBJ parsing"),MB_OK|MB_ICONEXCLAMATION);
#else
                        cerr << "Unrecognised line in OBJ file";
#endif
                    }
                    tokenised.clear();
                }
            }
            myfile.close();
            for (GLuint i = 0; i < m_indices.size(); i++) {
                m_vertices[m_indices[i]].normal[0] = vertex_normals[normal_indices[i] * 3];
                m_vertices[m_indices[i]].normal[1] = vertex_normals[normal_indices[i] * 3 + 1];
                m_vertices[m_indices[i]].normal[2] = vertex_normals[normal_indices[i] * 3 + 2];
            }
        } else {
#ifdef _WINDOWS
            MessageBox(NULL,TEXT("Unable to open file"),TEXT("Initialisation times"),MB_OK|MB_ICONEXCLAMATION);
#else
            cerr << "Unable to open file";
#endif
        }

    }

    GLboolean Mesh::Draw() {
        GLuint vertex_end = m_vertexoffset +  m_vertices.size() - 1;
        GLuint offset = m_indexoffset;

        vector<GLuint>::iterator it;
        for (it = m_faces.begin(); it < m_faces.end(); it++) {
            // Vertices are counter-clockwise, so need to draw as a triangle fan
            glDrawRangeElements (GL_TRIANGLE_FAN, m_vertexoffset, vertex_end, *it, GL_UNSIGNED_INT, reinterpret_cast<void*>(offset*sizeof(GLuint)));
            offset += *it;
        }

        return GL_TRUE;
    }

} // namespace Offender
