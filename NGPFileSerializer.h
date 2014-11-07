//===============================================================================================================
// NGPFileSerializer.h v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#ifndef NGPFILESERIALIZER_H
#define NGPFILESERIALIZER_H

#include <OgreSerializer.h>

class NGPFile;

class NGPFileSerializer : public Ogre::Serializer
	{
	public:
		NGPFileSerializer();
		virtual ~NGPFileSerializer();

		void exportNGPFile(const NGPFile *pText, const Ogre::String &fileName);
		void importNGPFile(Ogre::DataStreamPtr &stream, NGPFile *pDest);
	};

#endif
