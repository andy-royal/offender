#include "stdafx.h"

#ifndef _WINDOWS
#include <time.h>
#endif

//using namespace std;
//using namespace OpenGLUtils;

//#define WIREFRAME

namespace Offender {

    Terrain::Terrain(ObjMat* l_ViewPersp, GLuint l_width, GLuint l_depth, GLfloat l_twidth, GLfloat l_tdepth, GLfloat l_x, GLfloat l_z) {
        m_ViewPersp = l_ViewPersp;
        m_terrain_width = l_width;
        m_terrain_depth = l_depth;
        m_tile_width = l_twidth;
        m_tile_depth = l_tdepth;
        m_terrain_x = l_x;
        m_terrain_z = l_z;
        m_height_factor = m_tile_width * m_terrain_width / 2.0f;
        m_h = 1.0f;
        m_height_factor *= pow(2.0f, -m_h);
        Init();
    }

    Terrain::~Terrain() {
        glDeleteBuffers(1, &m_vertexBuffer);
        glDeleteBuffers(1, &m_indexBuffer);
        delete[] m_vertices;
        delete[] m_left;
        delete[] m_right;
        delete[] m_top;
        delete[] m_bottom;
    }

    GLboolean Terrain::Init() {
        //const GLuint m_terrain_width = 128;
        //const GLuint m_terrain_depth = 128;

        const string vShaderStr =
            "uniform mat4 u_MVPMatrix;                          \n"
            "uniform vec3 u_LightVector;                        \n"
            "uniform vec3 Ka; // Ambient reflectivity           \n"
            "uniform vec3 La;                                   \n"
            "uniform vec3 Kd; // Diffuse reflectivity           \n"
            "uniform vec3 Ld;                                   \n"
            "                                                   \n"
            "in vec3 a_Position;                                \n"
            "in vec3 a_Normal;                                  \n"
            "in vec2 a_TexCoord;                                \n"
//            "in vec4 a_Colour;                                  \n"
            "in vec3 a_TexWeightings;                           \n"
            "                                                   \n"
            "varying vec2 v_TexCoord;                           \n"
//            "varying vec4 v_Colour;                             \n"
            "varying vec3 v_TexWeightings;                      \n"
            "varying vec3 v_LightIntensity;                     \n"
            "                                                   \n"
            "void main()                                        \n"
            "{                                                  \n"
            "  gl_Position = u_MVPMatrix * vec4(a_Position, 1); \n"
            "  v_TexCoord = a_TexCoord;                         \n"
//            "  v_Colour = a_Colour;                             \n"
            "  v_TexWeightings = a_TexWeightings;               \n"
            "  v_LightIntensity = La * Ka + Ld * Kd * max(dot(a_Normal, u_LightVector), 0.0); \n"
//            "  v_LightIntensity = Ld * Kd;                      \n"
            "}                                                  \n";
   
#ifdef WIREFRAME
        const string fShaderStr =  
            "precision mediump float;                           \n"
            "                                                   \n"
            "in vec4 v_Colour;                                  \n"
            "                                                   \n"
            "void main()                                        \n"
            "{                                                  \n"
            "  gl_FragColor = vec4(0.0f, 0.30f, 0.15f, 1.0f);   \n"
            "}                                                  \n";
#else
        const string fShaderStr =  
            "precision mediump float;                             \n"
            "                                                     \n"
            "uniform sampler2D u_texture_0;                       \n"
            "uniform sampler2D u_texture_1;                       \n"
            "uniform sampler2D u_texture_2;                       \n"
            "                                                     \n"
            "in vec2 v_TexCoord;                                  \n"
//            "in vec4 v_Colour;                                    \n"
            "in vec3 v_TexWeightings;                             \n"
            "in vec3 v_LightIntensity;                            \n"
            "                                                     \n"
            "void main()                                          \n"
            "{                                                    \n"
            "  vec4 grass = v_TexWeightings[0] * texture2D(u_texture_0, v_TexCoord); \n"
            "  vec4 rock  = v_TexWeightings[1] * texture2D(u_texture_1, v_TexCoord); \n"
            "  vec4 snow  = v_TexWeightings[2] * texture2D(u_texture_2, v_TexCoord); \n"
            "  gl_FragColor = vec4(v_LightIntensity, 1.0) * (grass + rock + snow);  \n"
//            "  gl_FragColor = grass + rock + snow;                \n"
//            "  gl_FragColor = v_Colour;                           \n"
            "}                                                    \n";
#endif

        map<int, string> AttrBindings;
        AttrBindings[10] = "a_Position";
        //AttrBindings[11] = "a_Colour";
        AttrBindings[11] = "a_Normal";
        AttrBindings[12] = "a_TexCoord";
        AttrBindings[13] = "a_TexWeightings";

        m_TerrainProgram = LoadProgram(vShaderStr, fShaderStr, AttrBindings);
        m_TerrainMVPHandle = glGetUniformLocation(m_TerrainProgram, "u_MVPMatrix");
        m_LightVectorHandle = glGetUniformLocation(m_TerrainProgram, "u_LightVector");
        m_AmbientReflectivityHandle = glGetUniformLocation(m_TerrainProgram, "Ka");
        m_AmbientIntensityHandle = glGetUniformLocation(m_TerrainProgram, "La");
        m_DiffuseReflectivityHandle = glGetUniformLocation(m_TerrainProgram, "Kd");
        m_DiffuseIntensityHandle = glGetUniformLocation(m_TerrainProgram, "Ld");

        m_TerrainSampler0Handle = glGetUniformLocation(m_TerrainProgram, "u_texture_0");
        m_TerrainSampler1Handle = glGetUniformLocation(m_TerrainProgram, "u_texture_1");
        m_TerrainSampler2Handle = glGetUniformLocation(m_TerrainProgram, "u_texture_2");

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_init_start_time = clock();
#endif

        unsigned int row, column;
        m_vertices = new vertexStruct[(m_terrain_width + 1) * (m_terrain_depth + 1)];
        m_left = new vertexStruct[m_terrain_depth + 1];
        m_right = new vertexStruct[m_terrain_depth + 1];
        m_top = new vertexStruct[m_terrain_width + 1];
        m_bottom = new vertexStruct[m_terrain_width + 1];
        vertexStruct sw_corner;
        vertexStruct se_corner;
        vertexStruct nw_corner;
        vertexStruct ne_corner;

        unsigned int final_row = m_terrain_depth;
        unsigned int final_col = m_terrain_width;
        unsigned int row_stride = final_row;
        unsigned int col_stride = final_col;

        SetVertex(0, 0, m_terrain_x, 0.0f, m_terrain_z);  // SW corner
        SetVertex(m_terrain_depth, 0, m_terrain_x, 0.0f, m_terrain_z + (m_tile_depth * m_terrain_depth));  // NW corner
        SetVertex(0, m_terrain_width, m_terrain_x + (m_tile_width * m_terrain_width), 0.0f, m_terrain_z);  // SE corner
        SetVertex(m_terrain_depth, m_terrain_width, m_terrain_x + (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + (m_tile_depth * m_terrain_depth));  // NE corner

        SetVertex(&m_left[0], m_terrain_x - (m_tile_width * m_terrain_width), 0.0f, m_terrain_z);
        SetVertex(&m_left[final_row], m_terrain_x - (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + (m_tile_depth * m_terrain_depth));
        SetVertex(&m_right[0], m_terrain_x + 2 * (m_tile_width * m_terrain_width), 0.0f, m_terrain_z);
        SetVertex(&m_right[final_row], m_terrain_x + 2 * (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + (m_tile_depth * m_terrain_depth));
        SetVertex(&m_bottom[0], m_terrain_x, 0.0f, m_terrain_z - (m_tile_depth * m_terrain_depth));
        SetVertex(&m_bottom[final_col], m_terrain_x + (m_tile_width * m_terrain_width), 0.0f, m_terrain_z - (m_tile_depth * m_terrain_depth));
        SetVertex(&m_top[0], m_terrain_x, 0.0f, m_terrain_z + 2 * (m_tile_depth * m_terrain_depth));
        SetVertex(&m_top[final_col], m_terrain_x + (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + 2 * (m_tile_depth * m_terrain_depth));
        SetVertex(&sw_corner, m_terrain_x - (m_tile_width * m_terrain_width), 0.0f, m_terrain_z - (m_tile_depth * m_terrain_depth));
        SetVertex(&se_corner, m_terrain_x + 2 * (m_tile_width * m_terrain_width), 0.0f, m_terrain_z - (m_tile_depth * m_terrain_depth));
        SetVertex(&nw_corner, m_terrain_x - (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + 2 * (m_tile_depth * m_terrain_depth));
        SetVertex(&ne_corner, m_terrain_x + 2 * (m_tile_width * m_terrain_width), 0.0f, m_terrain_z + 2 * (m_tile_depth * m_terrain_depth));

        unsigned int dimension = 1;
        vertexStruct *origin, *sw, *nw, *se, *ne, *n, *e;
        float x, y, z;

        for (unsigned int iteration = 0; iteration < 7; iteration++) {
            unsigned int next_dimension = dimension * 2;
            unsigned int next_row_stride = row_stride / 2;
            unsigned int next_col_stride = col_stride / 2;

            // Diamond step
            for (column = 0; column < final_col; column+=col_stride) {
                for (row = 0; row < final_row; row+=row_stride) {
                    sw = GetVertex(row, column);
                    nw = GetVertex(row, column + col_stride);
                    se = GetVertex(row + row_stride, column);
                    ne = GetVertex(row + row_stride, column + col_stride);
                    x = (sw->position[0] + nw->position[0] + se->position[0] + ne->position[0]) / 4.0f;
                    y = (sw->position[1] + nw->position[1] + se->position[1] + ne->position[1]) / 4.0f;
                    z = (sw->position[2] + nw->position[2] + se->position[2] + ne->position[2]) / 4.0f;
                    y += m_height_factor * random_height_adjustment(x, z);
                    SetVertex(row + next_row_stride, column + next_col_stride, x, y, z);
                }
            }

            // Diamond step for boundaries
            for (row = 0; row < final_row; row+=row_stride) {
                DiamondSquare(&m_left[row + next_row_stride], &m_left[row], &m_left[row + row_stride], GetVertex(row, 0), GetVertex(row + row_stride, 0));
                DiamondSquare(&m_right[row + next_row_stride], GetVertex(row, final_col), GetVertex(row + row_stride, final_col), &m_right[row], &m_right[row + row_stride]);
            }
            for (column = 0; column < final_col; column+=col_stride) {
                DiamondSquare(&m_bottom[column + next_col_stride], &m_bottom[column], GetVertex(0, column), &m_bottom[column + col_stride], GetVertex(0, column + col_stride));
                DiamondSquare(&m_top[column + next_col_stride], &m_top[column], GetVertex(final_row, column), &m_top[column + col_stride], GetVertex(final_row, column + col_stride));
            }
            DiamondSquare(&sw_corner, &sw_corner, &m_left[0], &m_bottom[0], GetVertex(0,0));
            DiamondSquare(&se_corner, &m_bottom[final_col], &se_corner, GetVertex(0, final_col), &m_right[0]);
            DiamondSquare(&nw_corner, &m_left[final_row], GetVertex(final_row, 0), &nw_corner, &m_top[0]);
            DiamondSquare(&ne_corner, GetVertex(final_row, final_col), &m_right[final_row], &m_top[final_col], &ne_corner);

            // Square step
            for (column = 0; column <= final_col; column+=col_stride) {
                for (row = 0; row <= final_row; row+=row_stride) {
                    origin = GetVertex(row, column);
                    if (row != final_row) {
                        if (column == 0) 
                            nw     = &m_left[row + next_row_stride];
                        else
                            nw     = GetVertex(row + next_row_stride, column - next_col_stride);
                        n      = GetVertex(row + row_stride, column);
                    }
                    if (row != final_row && column != final_col)
                        ne     = GetVertex(row + next_row_stride, column + next_col_stride);
                    else if (row != final_row)
                        ne     = &m_right[row + next_row_stride];
                    else if (column != final_col)
                        ne     = &m_top[column + next_col_stride];
                    if (column != final_col) {
                        e      = GetVertex(row, column + col_stride);
                        if (row == 0)
                            se     = &m_bottom[column + next_col_stride];
                        else
                            se     = GetVertex(row - next_row_stride, column + next_col_stride);
                    }
                    if (row != final_row) {
                        float x = (origin->position[0] + n->position[0]) / 2.0f;
                        float y = (origin->position[1] + nw->position[1] + n->position[1] + ne->position[1]) / 4.0f;
                        float z = (origin->position[2] + n->position[2]) / 2.0f;
                        y += m_height_factor * random_height_adjustment(x, z);
                        SetVertex(row + next_row_stride, column, x, y, z);
                    }
                    if (column != final_col) {
                        float x = (origin->position[0] + e->position[0]) / 2.0f;
                        float y = (origin->position[1] + ne->position[1] + e->position[1] + se->position[1]) / 4.0f;
                        float z = (origin->position[2] + e->position[2]) / 2.0f;
                        y += m_height_factor * random_height_adjustment(x, z);
                        SetVertex(row, column + next_col_stride, x, y, z);
                    }
                }
            }

            // Square step for boundaries
            for (row = 0; row <= final_row; row+=row_stride) {
                if (row == 0) {
                    DiamondSquare(&m_left[row], &m_left[row], GetVertex(row, 0), &sw_corner, &m_left[row + next_row_stride]);
                    DiamondSquare(&m_right[row], GetVertex(row, final_col), &m_right[row], &se_corner, &m_right[row + next_row_stride]);
                } else if (row == final_row) {
                    DiamondSquare(&m_left[row], &m_left[row], GetVertex(row, 0), &m_left[row - next_row_stride], &nw_corner);
                    DiamondSquare(&m_right[row], GetVertex(row, final_col), &m_right[row], &m_right[row - next_row_stride], &ne_corner);
                } else {
                    DiamondSquare(&m_left[row], &m_left[row], GetVertex(row, 0), &m_left[row - next_row_stride], &m_left[row + next_row_stride]);
                    DiamondSquare(&m_right[row], GetVertex(row, final_col), &m_right[row], &m_right[row - next_row_stride], &m_right[row + next_row_stride]);
                }
            }
            for (column = 0; column <= final_col; column+=col_stride) {
                if (column == 0) {
                    DiamondSquare(&m_bottom[column], &sw_corner, &m_bottom[column + next_col_stride], &m_bottom[column], GetVertex(0, column));
                    DiamondSquare(&m_top[column], &nw_corner, &m_top[column + next_col_stride], GetVertex(final_row, column), &m_top[column]);
                } else if (column == final_col) {
                    DiamondSquare(&m_bottom[column], &m_bottom[column - next_col_stride], &se_corner, &m_bottom[column], GetVertex(0, column));
                    DiamondSquare(&m_top[column], &m_top[column - next_col_stride], &ne_corner, GetVertex(final_row, column), &m_top[column]);
                } else {
                    DiamondSquare(&m_bottom[column], &m_bottom[column - next_col_stride], &m_bottom[column + next_col_stride], &m_bottom[column], GetVertex(0, column));
                    DiamondSquare(&m_top[column], &m_top[column - next_col_stride], &m_top[column + next_col_stride], GetVertex(final_row, column), &m_top[column]);
                }
            }

            dimension = next_dimension;
            row_stride = next_row_stride;
            col_stride = next_col_stride;
            m_height_factor *= pow(2.0f, -m_h);
            //uint32_t a = LCG(-320.0f, -320.0f);
            //uint32_t b = LCG(320.0f, -320.0f);
            //uint32_t c = LCG(-320.0f, 320.0f);
            //uint32_t d = LCG(320.0f, 320.0f);
            //float e = 0.0f;
        }

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_vertices_done_time = clock();
#endif
        // Calculate vertex normals
        // First, calculate face normals and add them to each vertex
        // Faces in this block of terrain
        for (row = 0; row < final_row; row++) {
            for (column = 0; column < final_col; column++) {
                AddFaceNormalsToVertices(GetVertex(row, column),
                                         GetVertex(row, column+1),
                                         GetVertex(row+1, column),
                                         GetVertex(row+1, column+1));
            }
        }
        // Faces in first column either side
        for (row = 0; row < final_row; row++) {
            AddFaceNormalsToVertices(&m_left[row],
                                     GetVertex(row, 0),
                                     &m_left[row+1],
                                     GetVertex(row+1, 0));
            AddFaceNormalsToVertices(GetVertex(row, final_col),
                                     &m_right[row],
                                     GetVertex(row+1, final_col),
                                     &m_right[row+1]);
        }
        // Faces in first row top and bottom
        for (column = 0; column < final_col; column++) {
            AddFaceNormalsToVertices(&m_bottom[column],
                                     &m_bottom[column+1],
                                     GetVertex(0, column),
                                     GetVertex(0, column+1));
            AddFaceNormalsToVertices(GetVertex(final_row, column),
                                     GetVertex(final_row, column+1),
                                     &m_top[column],
                                     &m_top[column+1]);
        }
        // Faces in the corners
        AddFaceNormalsToVertices(&sw_corner,
                                 &m_bottom[0],
                                 &m_left[0],
                                 GetVertex(0, 0));
        AddFaceNormalsToVertices(&m_bottom[final_col],
                                 &se_corner,
                                 GetVertex(0, final_col),
                                 &m_right[0]);
        AddFaceNormalsToVertices(&m_left[final_row],
                                 GetVertex(final_row, 0),
                                 &nw_corner,
                                 &m_top[0]);
        AddFaceNormalsToVertices(GetVertex(final_row, final_col),
                                 &m_right[final_row],
                                 &m_top[final_col],
                                 &ne_corner);

        // Normalise all vertex normals
        for (row = 0; row <= final_row; row++) {
            for (column = 0; column <= final_col; column++) {
                vertexStruct* l_vert = GetVertex(row, column);
                ObjVec l_normalised = ObjVec(l_vert->normal).Normalise();
                l_vert->normal[0] = l_normalised[0];
                l_vert->normal[1] = l_normalised[1];
                l_vert->normal[2] = l_normalised[2];
            }
        }

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_normals_done_time = clock();
#endif

#ifdef WIREFRAME
        auto_ptr<GLuint> indices(new GLuint[2 * (m_terrain_width + 1) * (m_terrain_depth + 1)]);
        for (GLuint point = 0; point < ((m_terrain_width + 1) * (m_terrain_depth + 1)); point++) {
            indices.get()[point] = point;
        }
        for (row = 0; row <= m_terrain_depth; row++) {
            for (column = 0; column <= m_terrain_width; column++) {
                indices.get()[((m_terrain_width + 1) * (m_terrain_depth + 1)) + column + (row * (m_terrain_width+1))] = column * (m_terrain_depth+1) + row;
            }
        }
#else
        auto_ptr<GLuint> indices(new GLuint[2 * m_terrain_width * (m_terrain_depth + 1)]);
        for (column = 0; column < m_terrain_width; column++) {
            for (row = 0; row < (m_terrain_depth+1); row++) {
                indices.get()[row * 2 + column * (m_terrain_depth+1) * 2] = column * (m_terrain_depth+1) + row;
                indices.get()[row * 2 + column * (m_terrain_depth+1) * 2 + 1] = (column+1) * (m_terrain_depth+1) + row;
            }
        }
#endif

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_indices_done_time = clock();
#endif

        m_TerrainTexture0Handle = CreateTextureFromFile("grass-texture-0.jpg");
        m_TerrainTexture1Handle = CreateTextureFromFile("Red-rock-texture-5.jpg");
        m_TerrainTexture2Handle = CreateTextureFromFile("snow.jpg");

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_textures_done_time = clock();
#endif
        glGenBuffers(1, &m_vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, (m_terrain_width + 1) * (m_terrain_depth + 1) * sizeof(vertexStruct), m_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &m_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
#ifdef WIREFRAME
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * (m_terrain_width + 1) * (m_terrain_depth + 1) * sizeof(GLuint), indices.get(), GL_STATIC_DRAW);
#else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * m_terrain_width * (m_terrain_depth + 1) * sizeof(GLuint), indices.get(), GL_STATIC_DRAW);
#endif
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#ifdef MEASURE_TERRAIN_INIT_TIMES
        m_init_done_time = clock();
#endif
        return GL_TRUE;
    }

    GLboolean Terrain::Draw() {
        // Use the program object
        glUseProgram (m_TerrainProgram);

        // Load the vertex data
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
        glVertexAttribPointer (10, 3, GL_FLOAT, GL_FALSE, sizeof(vertexStruct), reinterpret_cast<void*>(offsetof(vertexStruct,position)));
        glEnableVertexAttribArray (10);
        glVertexAttribPointer (11, 3, GL_FLOAT, GL_FALSE, sizeof(vertexStruct), reinterpret_cast<void*>(offsetof(vertexStruct,normal)));
        glEnableVertexAttribArray (11);
        // Load the colour data
        glVertexAttribPointer (12, 2, GL_FLOAT, GL_FALSE, sizeof(vertexStruct), reinterpret_cast<void*>(offsetof(vertexStruct,texcoord)));
        glEnableVertexAttribArray (12);
        glVertexAttribPointer (13, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertexStruct), reinterpret_cast<void*>(offsetof(vertexStruct,texweightings)));
        glEnableVertexAttribArray (13);

        glUniformMatrix4fv(m_TerrainMVPHandle, 1, GL_TRUE, m_ViewPersp->data());
        ObjVec LightVector(0.5f, sqrt(3.0f) / 2.0f, 0.0f);
//        glUniform3fv(m_LightVectorHandle, 1, LightVector.data());
        glUniform3f(m_LightVectorHandle, 0.5f, sqrt(3.0f) / 2.0f, 0.0f);
        glUniform3f(m_AmbientReflectivityHandle, 1.0f, 1.0f, 1.0f);
        glUniform3f(m_AmbientIntensityHandle, 0.2f, 0.2f, 0.2f);
        glUniform3f(m_DiffuseReflectivityHandle, 1.0f, 1.0f, 1.0f);
        glUniform3f(m_DiffuseIntensityHandle, 0.8f, 0.8f, 0.8f);

        // Bind the textures
        glActiveTexture (GL_TEXTURE0);
        glBindTexture (GL_TEXTURE_2D, m_TerrainTexture0Handle);
        glUniform1i (m_TerrainSampler0Handle, 0);
        glActiveTexture (GL_TEXTURE1);
        glBindTexture (GL_TEXTURE_2D, m_TerrainTexture1Handle);
        glUniform1i (m_TerrainSampler1Handle, 1);
        glActiveTexture (GL_TEXTURE2);
        glBindTexture (GL_TEXTURE_2D, m_TerrainTexture2Handle);
        glUniform1i (m_TerrainSampler2Handle, 2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
#ifdef WIREFRAME
        GLuint column, row;
        for (column=0; column <= m_terrain_width; column++) {
            glDrawRangeElements (GL_LINE_STRIP,
                                 column * (m_terrain_depth+1),
                                 (column+1) * (m_terrain_depth+1),
                                 m_terrain_depth+1,
                                 GL_UNSIGNED_INT,
                                 reinterpret_cast<void*>(column*(m_terrain_depth+1)*sizeof(GLuint)));
        }
        for (row=0; row <= m_terrain_depth; row++) {
            glDrawElements (GL_LINE_STRIP,
                            m_terrain_width+1,
                            GL_UNSIGNED_INT,
                            reinterpret_cast<void*>((((m_terrain_width + 1) * (m_terrain_depth + 1)) + (row * (m_terrain_width+1))) * sizeof(GLuint)));
        }
#else
        GLuint column;
        for (column=0; column < m_terrain_width; column++) {
            GLuint vertices_per_strip = (m_terrain_depth+1) * 2;
            glDrawRangeElements (GL_TRIANGLE_STRIP,
                                 column * vertices_per_strip,
                                 (column+2) * vertices_per_strip,
                                 vertices_per_strip,
                                 GL_UNSIGNED_INT,
                                 reinterpret_cast<void*>(column*vertices_per_strip*sizeof(GLuint)));
        }
#endif
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return GL_TRUE;
    }

    GLboolean Terrain::CheckForCollision(Object* l_object) {
        ObjPos object_position = l_object->GetPosition();
        if (object_position.x() < m_terrain_x || object_position.x() > (m_terrain_x + m_terrain_width*m_tile_width) ||
            object_position.z() < m_terrain_z || object_position.z() > (m_terrain_z + m_terrain_depth*m_tile_depth)) {
            // Outside the defined terrain
            return GL_FALSE;
        }
        GLuint tile_x = static_cast<GLuint>((object_position.x() - m_terrain_x) / m_tile_width);
        GLuint tile_z = static_cast<GLuint>((object_position.z() - m_terrain_z) / m_tile_depth);

        if (CheckForTriangleCollision(tile_x      * (m_terrain_depth+1) + tile_z,         // SW
                                     (tile_x + 1) * (m_terrain_depth+1) + tile_z,         // SE
                                      tile_x      * (m_terrain_depth+1) + tile_z + 1,     // NW
                                      l_object) ||
            CheckForTriangleCollision((tile_x + 1) * (m_terrain_depth+1) + tile_z,         // SE
                                       tile_x      * (m_terrain_depth+1) + tile_z + 1,     // NW
                                      (tile_x + 1) * (m_terrain_depth+1) + tile_z + 1,     // NE
                                       l_object)) {
            return GL_TRUE;
        }
        return GL_FALSE;
    }

    GLboolean Terrain::CheckForTriangleCollision (GLuint a, GLuint b, GLuint c, Object* l_object) {
        vector<ObjPos> corners;
        corners.reserve(3);
        //vertexStruct* m_vertices;
        corners.push_back(m_vertices[a].position);
        corners.push_back(m_vertices[b].position);
        corners.push_back(m_vertices[c].position);
        
        // Measure distance from centre-point of object to plane of triangle
        ObjPos object_position = l_object->GetPosition();
        OBJ_NUMTYPE object_collision_radius = l_object->GetCollisionRadius();
        ObjVec vec1 = corners[1] - corners[0];
        ObjVec vec2 = corners[2] - corners[0];
        ObjVec vec3 = corners[2] - corners[1];
        ObjVec normal = cross_product(vec1, vec2).Normalise();
        ObjVec plane_to_point = object_position - corners[0];
        OBJ_NUMTYPE d = dot_product(plane_to_point, normal);

        if (abs(d) < object_collision_radius) {
            // Close enough to plane, but within the triangle?
            ObjPos shadow = object_position - d * normal;
            ObjVec corner0_to_shadow = cross_product(vec1, ObjVec(shadow - corners[0])); 
            ObjVec corner1_to_shadow = cross_product(vec3, ObjVec(shadow - corners[1]));
            ObjVec corner2_to_shadow = cross_product(-vec2, ObjVec(shadow - corners[2]));
            if ((dot_product(corner0_to_shadow, corner1_to_shadow) > 0) &&
                (dot_product(corner0_to_shadow, corner2_to_shadow) > 0)) {
                return GL_TRUE;  // Yah collision
            }
        }
        return GL_FALSE;  // No collision
    }

    void Terrain::SetVertex(unsigned int row, unsigned int col, GLfloat x, GLfloat y, GLfloat z) {
        unsigned int index = row + col * (m_terrain_depth + 1);
        SetVertex(&m_vertices[index], x, y, z);
        m_vertices[index].texcoord[0] = static_cast<GLfloat>(col) / static_cast<GLfloat>(m_terrain_width);
        m_vertices[index].texcoord[1] = static_cast<GLfloat>(row) / static_cast<GLfloat>(m_terrain_depth);
    }

    void Terrain::SetVertex(vertexStruct* l_vertex, GLfloat x, GLfloat y, GLfloat z) {
        l_vertex->position[0] = x;
        l_vertex->position[1] = y;
        l_vertex->position[2] = z;
        l_vertex->texweightings[0] = (y < -300.0f) ? 255 : 0;
        l_vertex->texweightings[1] = ((y >= -300.0f) && (y < 0.0f)) ? 255 : 0;
        l_vertex->texweightings[2] = (y >= 0.0f) ? 255 : 0;
        l_vertex->normal[0] = 0.0f;
        l_vertex->normal[1] = 0.0f;
        l_vertex->normal[2] = 0.0f;
    }

    vertexStruct* Terrain::GetVertex(unsigned int row, unsigned int col) {
        return &m_vertices[row + col * (m_terrain_depth + 1)];
    }

    void Terrain::DiamondSquare(vertexStruct* centre, vertexStruct* p1, vertexStruct* p2,
                                                                             vertexStruct* p3, vertexStruct* p4) {
        float x = (p1->position[0] + p2->position[0] + p3->position[0] + p4->position[0]) / 4.0f;
        float y = (p1->position[1] + p2->position[1] + p3->position[1] + p4->position[1]) / 4.0f;
        float z = (p1->position[2] + p2->position[2] + p3->position[2] + p4->position[2]) / 4.0f;
        y += m_height_factor * random_height_adjustment(x, z);
        SetVertex(centre, x, y, z);
    }

    void Terrain::AddNormal(vertexStruct* l_vert, ObjVec& l_norm) {
        l_vert->normal[0] += l_norm[0];
        l_vert->normal[1] += l_norm[1];
        l_vert->normal[2] += l_norm[2];
    }

    void Terrain::AddFaceNormalsToVertices(vertexStruct* bl, vertexStruct* br, vertexStruct* tl, vertexStruct* tr) {
        ObjVec diagonal = static_cast<ObjPos>(tl->position) - static_cast<ObjPos>(br->position);
        ObjVec bottom = static_cast<ObjPos>(bl->position) - static_cast<ObjPos>(br->position);
        ObjVec right = static_cast<ObjPos>(tr->position) - static_cast<ObjPos>(br->position);
//        ObjVec bl_norm = cross_product(diagonal, bottom);
//        ObjVec tr_norm = cross_product(right, diagonal);
        ObjVec bl_norm = cross_product(bottom, diagonal);
        ObjVec tr_norm = cross_product(diagonal, right);
        AddNormal(bl, bl_norm);
        AddNormal(br, bl_norm);
        AddNormal(tl, bl_norm);
        AddNormal(tr, tr_norm);
        AddNormal(br, tr_norm);
        AddNormal(tl, tr_norm);
    }

    float random_height_adjustment(float msb, float lsb) {
        uint32_t* msb32 = reinterpret_cast<uint32_t*>(&msb);
        uint32_t* lsb32 = reinterpret_cast<uint32_t*>(&lsb);
#ifdef DIAMOND_SQUARE_USES_CRC64
        uint64_t x0 = (static_cast<uint64_t>(*lsb32) << 32) + *msb32;
#else
        uint64_t x0 = (static_cast<uint64_t>(*msb32) << 32) + *lsb32;
#endif
#ifdef DIAMOND_SQUARE_USES_CRC_LCG
        uint32_t result = crc_lcg(reinterpret_cast<void*>(&x0), sizeof(uint64_t));
#else
#ifdef DIAMOND_SQUARE_USES_CRC64
        uint32_t result = crc32(reinterpret_cast<void*>(&x0), sizeof(uint64_t));
#else
        uint64_t x1 = LCG64(x0);
        uint32_t result = static_cast<uint32_t>(x1 >> 32);
#endif
#endif
        return static_cast<float>(static_cast<double>(result) / (uint64_t(1) << 31)) - 1.0f;
    }

    uint32_t LCG32(uint32_t x0) {
        uint32_t x1 = LCG32_A * x0 + LCG32_C;
        return x1;
    }

    uint64_t LCG64(uint64_t x0) {
        //uint64_t a = LCG_A;
        //uint64_t c = LCG_C;
        uint64_t x1 = LCG64_A * x0 + LCG64_C;
        return x1;
    }

    uint32_t crc32(uint32_t* a, uint32_t* b) {
        boost::crc_basic<32> crc(0x04c11db7, 0xffffffff, 0, false, false);
        crc.process_bytes(reinterpret_cast<void*>(a), sizeof(uint32_t));
        crc.process_bytes(reinterpret_cast<void*>(b), sizeof(uint32_t));
        return crc.checksum();
    }

    uint32_t crc32(void* data, size_t length) {
        boost::crc_basic<32> crc(0x04c11db7, 0xffffffff, 0, false, false);
        crc.process_bytes(data, length);
        return crc.checksum();
    }

    uint32_t crc_lcg(void* data, size_t length) {
        return LCG32(crc32(data, length));
    }

#ifdef MEASURE_TERRAIN_INIT_TIMES
    void Terrain::ReportTimes() {
        char message[1000];
        float arse = static_cast<float>(m_init_done_time - m_init_start_time)/CLOCKS_PER_SEC;
        sprintf(message, "Vertex generation %.3fs\n"
                         "Vertex normal calculation %.3fs\n"
                         "Index calculation %.3fs\n"
                         "Texture loading %.3fs\n"
                         "Buffer copying %.3fs\n"
                         "Total initialisation time %.3fs\n",
            static_cast<float>(m_vertices_done_time - m_init_start_time)/CLOCKS_PER_SEC,
            static_cast<float>(m_normals_done_time - m_vertices_done_time)/CLOCKS_PER_SEC,
            static_cast<float>(m_indices_done_time - m_normals_done_time)/CLOCKS_PER_SEC,
            static_cast<float>(m_textures_done_time - m_indices_done_time)/CLOCKS_PER_SEC,
            static_cast<float>(m_init_done_time - m_textures_done_time)/CLOCKS_PER_SEC,
            static_cast<float>(m_init_done_time - m_init_start_time)/CLOCKS_PER_SEC);
#ifdef _WINDOWS
        OLECHAR winfo[MAX_PATH+1];
        MultiByteToWideChar(CP_ACP, 0, message, -1, winfo, MAX_PATH);

        MessageBox(NULL,winfo,TEXT("Initialisation times"),MB_OK|MB_ICONEXCLAMATION);
#else
        cerr << message;
#endif
    }
#endif

} // namespace Offender
