/*
 *  TriangleMesh.h
 *  Refactored originally from The OpenGL SuperBible
 *
Copyright (c) 2007-2009-2023, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Neither the name of Richard S. Wright Jr. nor the names of other contributors may be used 
to endorse or promote products derived from this software without specific prior 
written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 *  This class allows you to simply add triangles as if this class were a 
 *  container. The AddTriangle() function searches the current list of triangles
 *  and determines if the vertex/normal/texcoord is a duplicate. If so, it addes
 *  an entry to the index array instead of the list of vertices.
 *  When finished, call EndMesh() to free up extra unneeded memory that is reserved
 *  as workspace when you call BeginMesh().
 *
 *  This class can easily be extended to contain other vertex attributes, and to 
 *  save itself and load itself from disk (thus forming the beginnings of a custom
 *  model file format).
 *
 */

#ifndef __TRIANGLE_MESH
#define __TRIANGLE_MESH


#include "math3d.h"
#include <cstdint>
#include <stdio.h>


class M3DTriangleMesh
    {
    public:
        M3DTriangleMesh(void);
        virtual ~M3DTriangleMesh(void);
        
        // Use these three functions to add triangles
        void BeginMesh(uint32_t nMaxVerts);
        void AddTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3], float epsilon = 0.0000001f);
        void End(void);

        // Useful for statistics
        inline uint32_t GetIndexCount(void) { return nNumIndexes; }
        inline uint32_t GetVertexCount(void) { return nNumVerts; }

        const M3DVector3f*    getVertexPositions(void) { return pVerts; }
        const M3DVector3f*    getVertexNormals(void) { return pNorms; }
        const M3DVector2f*    getVertexTexCoords(void) { return pTexCoords; }
        const uint16_t*       getVertexIndicies(void) { return pIndexes; }

        inline float GetBoundingSphere(void) { return boundingSphereRadius; }

        // Save to a single file
		bool SaveMesh(const char *szFileName);
		bool LoadMesh(const char *szFileName, bool bNormals = true, bool bTexCoords = true);
        
        // Stream to an existing file
        bool SaveMesh(FILE *pFile);
        bool LoadMesh(FILE *pFile, bool bNormals = true, bool bTexCoords = true);
        
    protected:
        uint16_t    *pIndexes;      // Array of indexes
        M3DVector3f *pVerts;        // Array of vertices
        M3DVector3f *pNorms;        // Array of normals
        M3DVector2f *pTexCoords;    // Array of texture coordinates
        
        uint32_t nMaxIndexes;         // Maximum workspace
        uint32_t nNumIndexes;         // Number of indexes currently used
        uint32_t nNumVerts;           // Number of vertices actually used
        
        bool   bMadeStuff;

        float	boundingSphereRadius;
    };

#endif
