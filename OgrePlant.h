//===============================================================================================================
// OgrePlant.h v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#ifndef OGREPLANT_H
#define OGREPLANT_H

#include <string>
#include <OgreMain/Ogre.h> // kshepherd added ogre and C++ includes needed
#include <OgreMain/OgreString.h>
#include <OgreMain/OgreResourceGroupManager.h>

#define OGRE_LOG(message) Ogre::LogManager::getSingleton().logMessage(message)

class P3DMaterialDef;

class PlantManager
	{
	private:
		const Ogre::String createMaterial(const std::string &baseName, const int groupIndex,
		                                  const P3DMaterialDef *material, bool &textured,
		                                  const Ogre::String &resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	public:
		bool serializePlant(Ogre::MeshPtr mesh_ptr);
		bool loadPlant(const std::string &filename, unsigned int seed = 0,
		               const Ogre::String &resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::MeshPtr loadPlantAsMesh(const std::string &filename, unsigned int seed = 0,
		                              const Ogre::String &resourceGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	};

#endif
