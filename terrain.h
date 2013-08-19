#pragma once
#include "stdafx.h"

#ifndef _WINDOWS
#ifdef _DEBUG
#include <time.h>
#endif
#endif

//using namespace std;
//using namespace OpenGLUtils;

//#define WIREFRAME

//#define DIAMOND_SQUARE_USES_CRC64
#define DIAMOND_SQUARE_USES_CRC_LCG
//#define MEASURE_TERRAIN_INIT_TIMES

namespace Offender {

    typedef struct vertexStruct
    {
        GLfloat position[3];
        //GLubyte colour[4];
        GLfloat texcoord[2];
        GLbyte texweightings[3];
        GLfloat normal[3];
    } vertexStruct;

    class Terrain {
        private:
            ObjMat*                  m_ViewPersp;
            GLuint                   m_terrain_width;
            GLuint                   m_terrain_depth;
            GLfloat                  m_tile_width;
            GLfloat                  m_tile_depth;
            GLfloat                  m_terrain_x;
            GLfloat                  m_terrain_z;
            GLuint                   m_TerrainProgram;
            GLuint                   m_TerrainMVPHandle;
            GLuint                   m_LightVectorHandle;
            GLuint                   m_AmbientReflectivityHandle;
            GLuint                   m_AmbientIntensityHandle;
            GLuint                   m_DiffuseReflectivityHandle;
            GLuint                   m_DiffuseIntensityHandle;
            GLuint                   m_TerrainTexture0Handle;
            GLuint                   m_TerrainTexture1Handle;
            GLuint                   m_TerrainTexture2Handle;
            GLuint                   m_TerrainSampler0Handle;
            GLuint                   m_TerrainSampler1Handle;
            GLuint                   m_TerrainSampler2Handle;
            GLfloat                  m_height_factor;
            GLfloat                  m_h;
            vertexStruct*            m_vertices;
            vertexStruct*            m_left;
            vertexStruct*            m_right;
            vertexStruct*            m_top;
            vertexStruct*            m_bottom;
            vertexStruct             sw_corner;
            vertexStruct             se_corner;
            vertexStruct             nw_corner;
            vertexStruct             ne_corner;
            GLuint                   m_vertexBuffer;
            GLuint                   m_indexBuffer;
            GLuint                   m_vao;
#ifdef MEASURE_TERRAIN_INIT_TIMES
            clock_t                  m_init_start_time;
            clock_t                  m_vertices_done_time;
            clock_t                  m_normals_done_time;
            clock_t                  m_indices_done_time;
            clock_t                  m_textures_done_time;
            clock_t                  m_init_done_time;
#endif
            GLboolean                Init();
            GLboolean                CheckForTriangleCollision (GLuint, GLuint, GLuint, Object*);
        public:
            Terrain(ObjMat*, GLuint, GLuint, GLfloat, GLfloat, GLfloat, GLfloat);
            ~Terrain();
            GLboolean                Draw();
            GLboolean                CheckForCollision(Object*);
            void                     SetVertex(unsigned int, unsigned int, GLfloat, GLfloat, GLfloat);
            void                     SetVertex(vertexStruct*, GLfloat, GLfloat, GLfloat);
            vertexStruct*            GetVertex(unsigned int, unsigned int);
            void                     DiamondSquare(vertexStruct*, vertexStruct*, vertexStruct*, vertexStruct*, vertexStruct*);
            void                     AddNormal(vertexStruct*, ObjVec&);
            void                     AddFaceNormalsToVertices(vertexStruct*, vertexStruct*, vertexStruct*, vertexStruct*);
#ifdef MEASURE_TERRAIN_INIT_TIMES
            void                     ReportTimes();
#endif
    };

    // Values from Numerical Recipes
    #define LCG32_A 1664525 
    #define LCG32_C 1013904223
    // Vlaues from MMIX
    #define LCG64_A 6364136223846793005
    #define LCG64_C 1442695040888963407

    float random_height_adjustment(float, float);

    uint32_t LCG32(uint32_t);
    uint64_t LCG64(uint64_t);
    uint32_t crc32(uint32_t*, uint32_t*);
    uint32_t crc32(void*, size_t);
    uint32_t crc_lcg(void*, size_t);

} // namespace Offender
