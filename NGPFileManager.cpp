//===============================================================================================================
// NGPFileManager.cpp v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#include "NGPFileManager.h"

#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 8) // kshepherd
template<> NGPFileManager *Ogre::Singleton<NGPFileManager>::msSingleton = 0;
#else
template<> NGPFileManager *Ogre::Singleton<NGPFileManager>::ms_Singleton = 0;
#endif

NGPFileManager *NGPFileManager::getSingletonPtr()
	{
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 8) // kshepherd
	return msSingleton;
#else
	return ms_Singleton;
#endif
	}

NGPFileManager &NGPFileManager::getSingleton()
	{
#if OGRE_VERSION_MAJOR > 1 || (OGRE_VERSION_MAJOR == 1 && OGRE_VERSION_MINOR >= 8) // kshepherd
	assert(msSingleton);
	return(*msSingleton);
#else
	assert(ms_Singleton);
	return(*ms_Singleton);
#endif
	}

NGPFileManager::NGPFileManager()
	{
	mResourceType = "NGPFile";

	// low, because it will likely reference other resources
	mLoadOrder = 30.0f;

	// register the ResourceManager with OGRE
	Ogre::ResourceGroupManager::getSingleton()._registerResourceManager(mResourceType, this);
	}

NGPFileManager::~NGPFileManager()
	{
	// unregister from OGRE
	Ogre::ResourceGroupManager::getSingleton()._unregisterResourceManager(mResourceType);
	}

NGPFilePtr NGPFileManager::load(const Ogre::String &name, const Ogre::String &group)
	{
	NGPFilePtr textf = getByName(name);

	if(textf.isNull())
		textf = create(name, group);

	textf->load();
	return textf;
	}

Ogre::Resource *NGPFileManager::createImpl(const Ogre::String &name, Ogre::ResourceHandle handle,
        const Ogre::String &group, bool isManual, Ogre::ManualResourceLoader *loader,
        const Ogre::NameValuePairList *createParams)
	{
	return new NGPFile(this, name, handle, group, isManual, loader);
	}
