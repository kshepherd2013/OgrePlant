//===============================================================================================================
// NGPFileSerializer.cpp v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#include "NGPFileSerializer.h"
#include "NGPFile.h"

NGPFileSerializer::NGPFileSerializer()
	{
	}

NGPFileSerializer::~NGPFileSerializer()
	{
	}

void NGPFileSerializer::exportNGPFile(const NGPFile *pText, const Ogre::String &fileName)
	{
	std::ofstream outFile;
	outFile.open(fileName.c_str(), std::ios::out);
	outFile << pText->getString();
	outFile.close();
	}

void NGPFileSerializer::importNGPFile(Ogre::DataStreamPtr &stream, NGPFile *pDest)
	{
	pDest->setString(stream->getAsString());
	}
