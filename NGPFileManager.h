//===============================================================================================================
// NGPFileManager.h v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#ifndef NGPFILEMANAGER_H
#define NGPFILEMANAGER_H

#include <OgreResourceManager.h>
#include "NGPFile.h"

class NGPFileManager : public Ogre::ResourceManager, public Ogre::Singleton<NGPFileManager>
	{
	protected:

		// From ResourceManager's interface
		Ogre::Resource *createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
		                           const Ogre::String &group, bool isManual,
		                           Ogre::ManualResourceLoader *loader,
		                           const Ogre::NameValuePairList *createParams);

	public:

		NGPFileManager();
		virtual ~NGPFileManager();

		virtual NGPFilePtr load(const Ogre::String &name, const Ogre::String &group);

		static NGPFileManager &getSingleton();
		static NGPFileManager *getSingletonPtr();
	};

#endif
