#ifndef __3DS_H
#define __3DS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _DEBUG
#pragma comment (lib, "3ds_d.lib")
#else
#pragma comment (lib, "3ds.lib")
#endif

#pragma pack(push)
#pragma pack(1)

typedef struct chunk3ds_s
{
	unsigned short	id;
	unsigned long	length;
} chunk3ds_t;

typedef struct material3ds_s
{
   char     name[32];
   float    ambient[4];
   float    diffuse[4];
   float    specular[4];
   float    emission[4];
   float    shininess;
   char     texture[32];
} material3ds_t;

typedef struct group3ds_s
{
   char     name[32];
   int      mat;
   int      start;
   int      size;
   long     *tris;
} group3ds_t;

typedef struct mesh3ds_s
{
   char           name[32];
   int            vertCount;
   int            texCoordCount;
   int            triCount;
   int            groupCount;
   float          (*verts)[3];
   float          (*norms)[3];
   float          (*texCoords)[2];
   float          (*tangentSpace)[9];
   long           (*tris)[3];
   group3ds_t     *groups;
   long           *smooth;
   float          axis[3][3];
   float          position[3];
   float          min[3];
   float          max[3];
   float          center[3];
} mesh3ds_t;

typedef struct data3ds_s
{
   int            materialCount;
   int            meshCount;
   int            vertCount;
   int            triCount;
   material3ds_t  *materials;
   mesh3ds_t      *meshes;
   float          min[3];
   float          max[3];
   float          center[3];
} data3ds_t;

#pragma pack(pop)

long Read3dsFile(char* pFilename, data3ds_t* output);
long Read3dsFileFast(char* pFilename, data3ds_t* output);
void Free3dsData(data3ds_t* data);
void Calculate3dsNormals(data3ds_t* model);
void Calculate3dsTangentSpace(data3ds_t* model);
void Calculate3dsBoundingBox(data3ds_t* model);

#endif