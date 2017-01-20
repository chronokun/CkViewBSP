#pragma once

struct TFileDirEntry
{
	int32_t m_iOffset;
	int32_t m_iLength;
};

struct TFileHeader
{
	int8_t m_Magic[4];
	int32_t m_iVersion;
	TFileDirEntry m_DirEntries[17];
};

//

struct TFileFace
{
	int32_t m_iTexture;
	int32_t m_iEffect;
	int32_t m_iType;
	int32_t m_iVertex;
	int32_t m_iNumVertices;
	int32_t m_iMeshVertex;
	int32_t m_iNumMeshVertices;
	int32_t m_iLightmapIndex;
	int32_t m_LightmapStart[2];
	int32_t m_LightmapSize[2];
	float m_LightmapOrigin[3];
	float m_LightmapVectors[2][3];
	float m_Normal[3];
	int32_t m_Size[2];
};

struct TFileVertex
{
	float m_Position[3];
	float m_TexCoord[2][2];
	float m_Normal[3];
	uint8_t m_Color[4];
};

//

struct TFileNode
{
	int32_t m_iPlane;
	int32_t m_Children[2];
	int32_t m_Mins[3];
	int32_t m_Maxs[3];
};

struct TFileLeaf
{
	int32_t m_iCluster;
	int32_t m_iArea;
	int32_t m_Mins[3];
	int32_t m_Maxs[3];
	int32_t m_iLeafFace;
	int32_t m_iNumLeafFaces;
	int32_t m_iLeafBrush;
	int32_t m_iNumLeafBrushes;
};

struct TFilePlane
{
	float m_Normal[3];
	float m_fDistance;
};

struct TFileLeafFace
{
	int32_t m_iFace;
};

struct TFileLeafBrush
{
	int32_t m_iBrush;
};

struct TFileVisData
{
	int32_t m_iNumVecs;
	int32_t m_iSizeVecs;
	std::vector<uint8_t> m_Vecs;
};
struct TFileLightMap
{
	uint8_t m_LM[128][128][3];
};

//

struct TQBSP
{
	std::vector<TFileFace> m_Faces;
	std::vector<TFileVertex> m_Vertices;
	std::vector<int32_t> m_MeshVerts;
	std::vector<TFilePlane> m_Planes;
	std::vector<TFileNode> m_Nodes;
	std::vector<TFileLeaf> m_Leafs;
	std::vector<TFileLeafFace> m_LeafFaces;
	std::vector<TFileLeafBrush> m_LeafBrushes;
	std::vector<TFileLightMap> m_LightMaps;
	TFileVisData m_VisData;
};

//

bool LoadQBSPFile(const char* _kpcFilename, TQBSP& _rQBSP);