//===============================================================================================================
// NGPFile.h v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#ifndef NGPFILE_H
#define NGPFILE_H

#include <OgreResourceManager.h>
#include <ngpcore/p3dhli.h>
#include <sstream>


class NGPFile : public Ogre::Resource, public P3DInputStringStream
	{
		Ogre::String mString;
		std::istringstream *mStream;
	protected:
		// From Ogre::Resource
		void loadImpl();
		void unloadImpl();
		size_t calculateSize() const;
	public:
		NGPFile(Ogre::ResourceManager *creator, const Ogre::String &name,
		        Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		        Ogre::ManualResourceLoader *loader = 0);
		virtual ~NGPFile();

		void setString(const Ogre::String &str);
		const Ogre::String &getString() const;
		// From P3DInputStringStream
		/** Read interface method required by ngplant
		 *
		 *  Reads one line at a time from the member string for ngplant to parse.
		 * \param Buffer Buffer that characters will be written to.
		 * \param BufferSize Size of buffer being filled.
		 */
		void ReadString(char *Buffer, unsigned int BufferSize);
		bool Eof() const;
	};

class NGPFilePtr : public Ogre::SharedPtr<NGPFile>
	{
	public:
		NGPFilePtr() : Ogre::SharedPtr<NGPFile>() {}
		explicit NGPFilePtr(NGPFile *rep) : Ogre::SharedPtr<NGPFile>(rep) {}
		NGPFilePtr(const NGPFilePtr &r) : Ogre::SharedPtr<NGPFile>(r) {}
		NGPFilePtr(const Ogre::ResourcePtr &r) : Ogre::SharedPtr<NGPFile>()
			{
			if(r.isNull())
				return;
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<NGPFile*>(r.getPointer());
			pUseCount = r.useCountPointer();
			useFreeMethod = r.freeMethod();
			if (pUseCount)
				{
				++(*pUseCount);
				}
			}

		/// Operator used to convert a ResourcePtr to an NGPFilePtr
		NGPFilePtr& operator=(const Ogre::ResourcePtr& r)
			{
			if(pRep == static_cast<NGPFile*>(r.getPointer()))
				return *this;
			release();
			if(r.isNull())
				return *this; // resource ptr is null, so the call to release above has done all we need to do.
			// lock & copy other mutex pointer
			OGRE_LOCK_MUTEX(*r.OGRE_AUTO_MUTEX_NAME)
			OGRE_COPY_AUTO_SHARED_MUTEX(r.OGRE_AUTO_MUTEX_NAME)
			pRep = static_cast<NGPFile*>(r.getPointer());
			pUseCount = r.useCountPointer();
			useFreeMethod = r.freeMethod();
			if (pUseCount)
				{
				++(*pUseCount);
				}
			return *this;
			}
	};

#endif
