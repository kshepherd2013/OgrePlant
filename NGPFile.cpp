//===============================================================================================================
// NGPFile.cpp v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#include "NGPFile.h"
#include "NGPFileSerializer.h"

NGPFile::NGPFile(Ogre::ResourceManager *creator, const Ogre::String &name,
                 Ogre::ResourceHandle handle, const Ogre::String &group,
                 bool isManual, Ogre::ManualResourceLoader *loader)
	: Ogre::Resource(creator, name, handle, group, isManual, loader)
	{
	}

NGPFile::~NGPFile()
	{
	unload();
	}

void NGPFile::loadImpl()
	{
	NGPFileSerializer serializer;
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(mName, mGroup,
	                             true, this);
	serializer.importNGPFile(stream, this);
	mStream = new std::istringstream(mString);
	}

void NGPFile::unloadImpl()
	{
	mString.clear();
	delete mStream;
	}

size_t NGPFile::calculateSize() const
	{
	// An approximation
	return mString.length() + sizeof(*mStream);
	}

void NGPFile::setString(const Ogre::String &str)
	{
	mString = str;
	}

const Ogre::String &NGPFile::getString() const
	{
	return mString;
	}

void NGPFile::ReadString(char *Buffer, unsigned int BufferSize)
	{
	mStream->getline(Buffer, BufferSize);
	// If reading DOS format file, stomp on carriage return byte
	if(Buffer[mStream->gcount()-2] == '\r')
		Buffer[mStream->gcount()-2] = '\0';
	}

bool NGPFile::Eof() const
	{
	return mStream->eof();
	}
