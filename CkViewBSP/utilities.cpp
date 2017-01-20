//
//	Author:	Michael Cameron
//	Email:	chronokun@hotmail.com
//

// Library Include
#include "libraries.h"
// This Include
#include "utilities.h"
// Local Includes

const bool LoadFileToVector(std::vector<char>& _rDest, const char* _kpcFilename)
{
	std::ifstream infile;
	
	infile.open(_kpcFilename, (std::ios::binary | std::ios::ate));
	if(infile.is_open())
	{
		unsigned int uiFileSize = (unsigned int)infile.tellg();
		if(uiFileSize == 0)
		{
			return(false);
		}
		_rDest.resize(uiFileSize);
		infile.seekg(0, std::ios::beg);
		infile.read(_rDest.data(), uiFileSize);
		infile.close();
		return(true);
	}
	else
	{
		return(false);
	}
}