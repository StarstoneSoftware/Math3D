/*
 *  TriangleMesh.cpp
 *

Copyright (c) 2007-2020, Richard S. Wright Jr.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list 
of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this list 
of conditions and the following disclaimer in the documentation and/or other 
materials provided with the distribution.

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
 * This class was refactored from GLTriangleBatch to just generate geometry, and to
 * format it as an interleaved array that could be used by either OpenGL or Vulkan.
 *
 */

#include "TriangleMesh.h"


///////////////////////////////////////////////////////////
// Constructor, does what constructors do... set everything to zero or NULL
TriangleMesh::TriangleMesh(void)
    {
    pIndexes = NULL;
    pVerts = NULL;
    pNorms = NULL;
    pTexCoords = NULL;
    
    nMaxIndexes = 0;
    nNumIndexes = 0;
    nNumVerts = 0;
    
    bMadeStuff = false;
	boundingSphereRadius = 0.0f;
    }
    
////////////////////////////////////////////////////////////
// Free any dynamically allocated memory. For those C programmers
// coming to C++, it is perfectly valid to delete a NULL pointer.
TriangleMesh::~TriangleMesh(void)
    {
    // Just in case these still are allocated when the object is destroyed
    delete [] pIndexes;
    delete [] pVerts;
    delete [] pNorms;
    delete [] pTexCoords;
    }
    
////////////////////////////////////////////////////////////
// Start assembling a mesh. You need to specify a maximum amount
// of indexes that you expect. The EndMesh will clean up any uneeded
// memory. This is far better than shreading your heap with STL containers...
// At least that's my humble opinion.
void TriangleMesh::BeginMesh(uint32_t nMaxVerts)
    {
    // Just in case this gets called more than once...
    delete [] pIndexes;
    delete [] pVerts;
    delete [] pNorms;
    delete [] pTexCoords;
    
    nMaxIndexes = nMaxVerts;
    nNumIndexes = 0;
    nNumVerts = 0;
    
    // Allocate new blocks. In reality, the other arrays will be
    // much shorter than the index array
    pIndexes = new uint32_t[nMaxIndexes];
    pVerts = new M3DVector3f[nMaxIndexes];
    pNorms = new M3DVector3f[nMaxIndexes];
    pTexCoords = new M3DVector2f[nMaxIndexes];
    }
  
/////////////////////////////////////////////////////////////////
// Add a triangle to the mesh. This searches the current list for identical
// (well, almost identical - these are floats you know...) verts. If one is found, it
// is added to the index array. If not, it is added to both the index array and the vertex
// array grows by one as well.
void TriangleMesh::AddTriangle(M3DVector3f verts[3], M3DVector3f vNorms[3], M3DVector2f vTexCoords[3], float epsilon)
    {
    // First thing we do is make sure the normals are unit length!
    // It's almost always a good idea to work with pre-normalized normals
    if(vNorms != NULL) {
        m3dNormalizeVector3(vNorms[0]);
        m3dNormalizeVector3(vNorms[1]);
        m3dNormalizeVector3(vNorms[2]);
        }
		
	
    // Search for match - triangle consists of three verts
    for(uint32_t iVertex = 0; iVertex < 3; iVertex++)
        {
        uint32_t iMatch = 0;
        for(iMatch = 0; iMatch < nNumVerts; iMatch++)
            {
            // We have vertexes, texture coordinates, and normals
			if(pTexCoords && pNorms) {
				if(m3dCloseEnough(pVerts[iMatch][0], verts[iVertex][0], epsilon) &&
				   m3dCloseEnough(pVerts[iMatch][1], verts[iVertex][1], epsilon) &&
				   m3dCloseEnough(pVerts[iMatch][2], verts[iVertex][2], epsilon) &&
					   
				   // AND the Normal is the same...
				   m3dCloseEnough(pNorms[iMatch][0], vNorms[iVertex][0], epsilon) &&
				   m3dCloseEnough(pNorms[iMatch][1], vNorms[iVertex][1], epsilon) &&
				   m3dCloseEnough(pNorms[iMatch][2], vNorms[iVertex][2], epsilon) &&
					   
					// And Texture is the same...
					m3dCloseEnough(pTexCoords[iMatch][0], vTexCoords[iVertex][0], epsilon) &&
					m3dCloseEnough(pTexCoords[iMatch][1], vTexCoords[iVertex][1], epsilon))
					{
					// Then add the index only
					pIndexes[nNumIndexes] = iMatch;
					nNumIndexes++;
					break;
					}
				}
            }
            
        // No match for this vertex, add to end of list
        if(iMatch == nNumVerts && nNumVerts < nMaxIndexes && nNumIndexes < nMaxIndexes)
            {
            memcpy(pVerts[nNumVerts], verts[iVertex], sizeof(M3DVector3f));
            memcpy(pNorms[nNumVerts], vNorms[iVertex], sizeof(M3DVector3f));
            memcpy(pTexCoords[nNumVerts], vTexCoords[iVertex], sizeof(M3DVector2f));
            
            pIndexes[nNumIndexes] = nNumVerts;
            nNumIndexes++; 
            nNumVerts++;
            }   
        }
    }
    


//////////////////////////////////////////////////////////////////
// Compact the data. This is a nice utility, but you should really
// save the results of the indexing for future use if the model data
// is static (doesn't change).
void TriangleMesh::End(void)
    {
    bMadeStuff = true;
    
	// Find the radius of the smallest sphere that would enclose the model
    // This is useful for some things.
	boundingSphereRadius = 0.0f;
	for(unsigned int i = 0; i < nNumVerts; i++) {
        float r = m3dGetVectorLengthSquared3(pVerts[i]);
		if(r > boundingSphereRadius)
			boundingSphereRadius = r;
		}
	boundingSphereRadius = sqrt(boundingSphereRadius);
    }


////////////////////////////////////////////////////////////////////////
// Save the mesh into the already open file stream
bool TriangleMesh::SaveMesh(FILE *pFile)
    {
    // Header contains...
    fwrite(&nNumIndexes, sizeof(uint32_t), 1, pFile);
    fwrite(&nNumVerts, sizeof(uint32_t), 1, pFile);
    fwrite(&boundingSphereRadius, sizeof(uint32_t), 1, pFile);
        
//    printf("Unique Verts: %d\r\nTriangles: %d\r\n\r\n", nNumVerts, nNumIndexes);

    fwrite(pIndexes, sizeof(uint32_t) * nNumIndexes, 1, pFile);
    
    // Vertex positions
    fwrite(pVerts, sizeof(M3DVector3f) * nNumVerts, 1, pFile);
    
    // Normals, if we have them
    if(pNorms) { // Look for original flag
        fwrite(pNorms, sizeof(M3DVector3f) * nNumVerts, 1, pFile);
        }
        
    // Texture Coordinates, if we have them
    if(pTexCoords)  // Look for original flag
        fwrite(pTexCoords, sizeof(M3DVector2f) * nNumVerts, 1, pFile);
        
    return true;
    }


////////////////////////////////////////////////////////////////////////////////////////////
// Load a mesh into this batch, given the existing and already opened file stream.
// So, You must have verts, but normals and vetexes are optional. You need to know
// ahead of time which are in the file.
bool TriangleMesh::LoadMesh(FILE *pFile, bool bNormals, bool bTexCoords)
    {
    // Read it all in
    fread(&nNumIndexes, sizeof(uint32_t), 1, pFile);
    fread(&nNumVerts, sizeof(uint32_t), 1, pFile);
    fread(&boundingSphereRadius, sizeof(uint32_t), 1, pFile);
    
    
    pIndexes = new uint32_t[nNumIndexes];
    fread(pIndexes, sizeof(uint32_t) * nNumIndexes, 1, pFile);
    
    pVerts = new M3DVector3f[nNumVerts];
    fread(pVerts, sizeof(M3DVector3f) * nNumVerts, 1, pFile);
    
    // Read Normals? If we have them, they occur before the texture coordinates
    if(bNormals) {
        pNorms = new M3DVector3f[nNumVerts];
        if(1 != fread(pNorms, sizeof(M3DVector3f) * nNumVerts, 1, pFile))
            { // dodo head, no normals
            delete [] pNorms;
            pNorms = NULL;
            }
        }
    
    // These are last, so when loading individual meshes from binary, it's okay if we
    // just run out of room. However, for multiple meshes in a single file, we need to
    // know if the mesh has texture coordinates or not. CAD models do not have texture
    // coordinates.
    if(bTexCoords) {
        pTexCoords = new M3DVector2f[nNumVerts];
        if(1 != fread(pTexCoords, sizeof(M3DVector2f) * nNumVerts, 1, pFile))
            {		// Sorry, no texture coordinates
            delete [] pTexCoords;
            pTexCoords = NULL;
            }   
        }
        
    return true;
    }


bool TriangleMesh::SaveMesh(const char *szFileName)
	{
	FILE *pFile;
	pFile = fopen(szFileName, "wb");
	if(pFile == NULL)
		return false;
        
    SaveMesh(pFile);

		
	fclose(pFile);
	return true;
	}


bool TriangleMesh::LoadMesh(const char *szFileName, bool bNormals, bool bTexCoords)
	{
	FILE *pFile = fopen(szFileName, "rb");
	if(pFile == NULL)
		return false;

    LoadMesh(pFile, bNormals, bTexCoords);

    fclose(pFile);

	return true;
	}
  
