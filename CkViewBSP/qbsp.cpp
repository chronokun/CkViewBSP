// Libraries Include
#include "libraries.h"
// This Include
#include "qbsp.h"

bool LoadQBSPFile(const char* _kpcFilename, TQBSP& _rQBSP)
{
	std::ifstream file (_kpcFilename, (std::ios::binary | std::ios::in | std::ios::ate));

	TFileHeader Header;

	if(file.is_open())
	{
		unsigned int uiSize = (unsigned int)file.tellg();
		char* pcMemBlock = new char[(unsigned int)uiSize];
		file.seekg(0, std::ios::beg);

		file.read(reinterpret_cast<char*>(&Header), sizeof(Header));

		bool bAbort = false;
		const char Magic[4] = {'I','B','S','P'};
		for(size_t i = 0; i < 4; i++)
		{
			if(Header.m_Magic[i] != Magic[i])
			{
				bAbort = true;
			}
		}
		if(Header.m_iVersion != 46)
		{
			bAbort = true;
		}
		if(bAbort)
		{
			file.close();
			return(false);
		}

		file.seekg(Header.m_DirEntries[2].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[2].m_iLength / sizeof(TFilePlane)); ++i)
		{
			TFilePlane Plane;
			file.read(reinterpret_cast<char*>(&Plane), sizeof(TFilePlane));
			_rQBSP.m_Planes.push_back(Plane);
		}

		file.seekg(Header.m_DirEntries[3].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[3].m_iLength / sizeof(TFileNode)); ++i)
		{
			TFileNode Node;
			file.read(reinterpret_cast<char*>(&Node), sizeof(TFileNode));
			_rQBSP.m_Nodes.push_back(Node);
		}

		file.seekg(Header.m_DirEntries[4].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[4].m_iLength / sizeof(TFileLeaf)); ++i)
		{
			TFileLeaf Leaf;
			file.read(reinterpret_cast<char*>(&Leaf), sizeof(TFileLeaf));
			_rQBSP.m_Leafs.push_back(Leaf);
		}

		file.seekg(Header.m_DirEntries[5].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[5].m_iLength / sizeof(TFileLeafFace)); ++i)
		{
			TFileLeafFace LeafFace;
			file.read(reinterpret_cast<char*>(&LeafFace), sizeof(TFileLeafFace));
			_rQBSP.m_LeafFaces.push_back(LeafFace);
		}

		file.seekg(Header.m_DirEntries[6].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[6].m_iLength / sizeof(TFileLeafBrush)); ++i)
		{
			TFileLeafBrush LeafBrush;
			file.read(reinterpret_cast<char*>(&LeafBrush), sizeof(TFileLeafBrush));
			_rQBSP.m_LeafBrushes.push_back(LeafBrush);
		}

		file.seekg(Header.m_DirEntries[10].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[10].m_iLength / sizeof(TFileVertex)); ++i)
		{
			TFileVertex Vertex;
			file.read(reinterpret_cast<char*>(&Vertex), sizeof(Vertex));
			_rQBSP.m_Vertices.push_back(Vertex);
		}

		file.seekg(Header.m_DirEntries[11].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[11].m_iLength / sizeof(int32_t)); ++i)
		{
			int32_t MeshVert;
			file.read(reinterpret_cast<char*>(&MeshVert), sizeof(MeshVert));
			_rQBSP.m_MeshVerts.push_back(MeshVert);
		}

		file.seekg(Header.m_DirEntries[13].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[13].m_iLength / sizeof(TFileFace)); ++i)
		{
			TFileFace Face;
			file.read(reinterpret_cast<char*>(&Face), sizeof(Face));
			_rQBSP.m_Faces.push_back(Face);
		}

		file.seekg(Header.m_DirEntries[14].m_iOffset, std::ios::beg);
		for(unsigned int i = 0; i < ((unsigned int)Header.m_DirEntries[14].m_iLength / sizeof(TFileLightMap)); ++i)
		{
			TFileLightMap LM;
			file.read(reinterpret_cast<char*>(&LM), sizeof(LM));
			_rQBSP.m_LightMaps.push_back(LM);
		}

		file.seekg(Header.m_DirEntries[16].m_iOffset, std::ios::beg);
		{
			file.read(reinterpret_cast<char*>(&_rQBSP.m_VisData.m_iNumVecs), sizeof(_rQBSP.m_VisData.m_iNumVecs));
			file.read(reinterpret_cast<char*>(&_rQBSP.m_VisData.m_iSizeVecs), sizeof(_rQBSP.m_VisData.m_iSizeVecs));
			_rQBSP.m_VisData.m_Vecs.resize(_rQBSP.m_VisData.m_iNumVecs * _rQBSP.m_VisData.m_iSizeVecs);
			file.read(reinterpret_cast<char*>(_rQBSP.m_VisData.m_Vecs.data()), sizeof(uint8_t) * _rQBSP.m_VisData.m_Vecs.size());
		}

		file.close();
	}
	else
	{
		return(false);
	}

	return(true);
}