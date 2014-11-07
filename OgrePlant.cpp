//===============================================================================================================
// OgrePlant.cpp v1.2
// Written by DragonM, this file is released into the public domain.
// Modified by kshepherd to work with Ogre 1.9.X, seperate serialization to disk functionality, include seed in file names.
// Wiki: http://www.ogre3d.org/tikiwiki/OgrePlant
// Previous documentation: http://www.ogre3d.org/forums/viewtopic.php?f=1&t=65025
// Public Source Repository: https://github.com/kshepherd2013/OgrePlant
//===============================================================================================================
#include "OgrePlant.h"
#include "NGPFileManager.h"

#include <ngpcore/p3dhli.h>

#define SUPPORT_NORMALS 1

bool PlantManager::serializePlant(Ogre::MeshPtr mesh_ptr)
	{
	bool result = false;
	if (!mesh_ptr.isNull())
		{
		Ogre::MaterialManager &mm = Ogre::MaterialManager::getSingleton();
		Ogre::MeshSerializer meshSerializer;
		Ogre::Mesh * mesh = mesh_ptr.getPointer();
		meshSerializer.exportMesh(mesh, mesh->getName());
		Ogre::Mesh::SubMeshIterator subMeshes = mesh->getSubMeshIterator();
		while (subMeshes.hasMoreElements())
			{
			Ogre::SubMesh * subMesh = subMeshes.getNext();
			if (subMesh != nullptr)
				{
				Ogre::MaterialPtr mat = mm.getByName(subMesh->getMaterialName());
				if (!mat.isNull())
					{
					Ogre::MaterialSerializer materialSerializer;
					materialSerializer.exportMaterial(mat, mat->getName());
					}
				}
			}
		}
	return result;
	}

bool PlantManager::loadPlant(const std::string &filename, unsigned int seed,
                             const Ogre::String &resourceGroup)
	{
	bool result = false;
	Ogre::MeshPtr mesh_ptr = loadPlantAsMesh(filename, seed, resourceGroup);
	if (!mesh_ptr.isNull())
		{
		result = serializePlant(mesh_ptr);
		}
	return result;
	}

Ogre::MeshPtr PlantManager::loadPlantAsMesh(const std::string &filename, unsigned int seed,
        const Ogre::String &resourceGroup)
	{
	bool result = false;
	Ogre::MeshPtr mesh;
	mesh.setNull();
	unsigned int branchGroupCount, totalPolys = 0;
	Ogre::MeshManager &mm = Ogre::MeshManager::getSingleton();
	NGPFileManager &nm = NGPFileManager::getSingleton();
	std::string mesh_filename = filename;
	if (seed != 0)
		{
		mesh_filename.append("_");
		mesh_filename.append(Ogre::StringConverter::toString(seed));
		}
	mesh_filename.append(".mesh");
	NGPFilePtr ngp = nm.load(filename, resourceGroup);
	try
		{
		P3DHLIPlantTemplate plantTemplate(ngp.getPointer());
		P3DHLIPlantInstance *plantInstance;
		plantInstance = plantTemplate.CreateInstance(seed);
		branchGroupCount = plantTemplate.GetGroupCount();
		mesh = mm.createManual(mesh_filename, resourceGroup);
		if(branchGroupCount > 0)
			{
			bool fullyTextured = true;
			for(unsigned int i = 0; i < branchGroupCount; ++i)
				{
				unsigned int branchCount = plantInstance->GetBranchCount(i);
				unsigned int vertexCount = plantInstance->GetVAttrCountI(
				                               i); // equal to template attrCount * instance branchCount
				unsigned int indexCount = plantTemplate.GetIndexCount(i, P3D_TRIANGLE_LIST);
				unsigned int branchAttrCount = plantTemplate.GetVAttrCountI(
				                                   i); // number of vertices in a single branch model
				bool textured = false;
				Ogre::SubMesh *submesh = mesh->createSubMesh();
				submesh->setMaterialName(createMaterial(filename, i, plantTemplate.GetMaterial(i), textured,
				                                        resourceGroup));
				submesh->useSharedVertices = false;
				submesh->vertexData = new Ogre::VertexData();
				submesh->vertexData->vertexStart = 0;
				submesh->vertexData->vertexCount = vertexCount;
				submesh->indexData->indexStart = 0;
				submesh->indexData->indexCount = indexCount * branchCount;
				totalPolys += submesh->indexData->indexCount/3;
				Ogre::VertexData *vertexData = submesh->vertexData;

				// Vertex format definitions...
				// ... for Ogre...
				Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;
				size_t currOffset = 0;
				vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
				currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
				vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
				currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
				if(textured)
					{
					vertexDecl->addElement(0, currOffset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
					currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
					}

				// ... and for ngplant.
				P3DHLIVAttrFormat format = P3DHLIVAttrFormat(currOffset);
				currOffset = 0;
				format.AddAttr(P3D_ATTR_VERTEX, currOffset);
				currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
				format.AddAttr(P3D_ATTR_NORMAL, currOffset);
				currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);
				if(textured)
					{
					format.AddAttr(P3D_ATTR_TEXCOORD0, currOffset);
					currOffset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT2);
					}

				// Allocate, lock, and get a pointer to the vertex buffer.
				Ogre::HardwareVertexBufferSharedPtr vBuf =
				    Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(vertexDecl->getVertexSize(0),
				            vertexCount, Ogre::HardwareBuffer::HBU_DYNAMIC, true);
				Ogre::VertexBufferBinding* binding = vertexData->vertexBufferBinding;
				binding->setBinding(0, vBuf);
				float* pVertex = static_cast<float*>(vBuf->lock(Ogre::HardwareBuffer::HBL_NORMAL));

				// Allocate, lock, and get a pointer to the index buffer.
				submesh->indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
				                                      Ogre::HardwareIndexBuffer::IT_16BIT, submesh->indexData->indexCount,
				                                      Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, false);
				Ogre::HardwareIndexBufferSharedPtr iBuf = submesh->indexData->indexBuffer;
				unsigned short *pIndices = static_cast<unsigned short *>(iBuf->lock(
				                               Ogre::HardwareBuffer::HBL_NORMAL));

				// Copy vertex data.
				plantInstance->FillVAttrBufferI((void *)pVertex, i, &format);
				// Flip UVs, if there are any.
				if(textured)
					{
					size_t offset = vertexDecl->findElementBySemantic(
					                    Ogre::VES_TEXTURE_COORDINATES)->getOffset()/sizeof(float);
					for(unsigned int j = 0; j < vertexCount*vertexDecl->getVertexSize(0)/sizeof(float);
					        j+=vertexDecl->getVertexSize(0)/sizeof(float))
						{
						pVertex[j+offset] = -pVertex[j+offset];
						pVertex[j+offset+1] = -pVertex[j+offset+1];
						}
					}
				// Copy index data.
				for(unsigned int branchIndex = 0; branchIndex < branchCount; ++branchIndex)
					{
					plantTemplate.FillIndexBuffer(&(pIndices[branchIndex * indexCount]), i, P3D_TRIANGLE_LIST,
					                              P3D_UNSIGNED_SHORT, branchIndex * branchAttrCount);
					}

				vBuf->unlock();
				iBuf->unlock();

				fullyTextured &= textured;
				}
			float minBound[3], maxBound[3];
			plantInstance->GetBoundingBox(minBound, maxBound);
			mesh->_setBounds(Ogre::AxisAlignedBox(minBound[0], minBound[1], minBound[2], maxBound[0],
			                                      maxBound[1], maxBound[2]));
			mesh->_setBoundingSphereRadius(mesh->getBounds().getHalfSize().length());
			mesh->load();
			unsigned short src, dest;
			if(fullyTextured && !mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
				{
				mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest, true, true);
				}
			result = true;

			Ogre::Mesh::SubMeshIterator smIt = mesh->getSubMeshIterator();
			while(smIt.hasMoreElements())
				{
				Ogre::SubMesh *sm = smIt.getNext();
				if(!sm->useSharedVertices)
					{
					Ogre::VertexDeclaration *autoDeclaration =
					    sm->vertexData->vertexDeclaration->getAutoOrganisedDeclaration(false, false,
					            false); // v1.1 change , false);
					if(*autoDeclaration != *(sm->vertexData->vertexDeclaration))
						{
						Ogre::BufferUsageList bufferUsages;
						for(size_t u = 0; u <= autoDeclaration->getMaxSource(); ++u)
							bufferUsages.push_back(Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
						sm->vertexData->reorganiseBuffers(autoDeclaration, bufferUsages);
						}
					}
				}
//      Ogre::MeshSerializer meshSerializer;
//      meshSerializer.exportMesh(mesh.getPointer(), mesh->getName());
			}
		else
			{
			OGRE_LOG("Loaded empty plant template " + filename);
			}
		delete plantInstance;
		}
	catch(const P3DException &exc)
		{
		OGRE_LOG("ngplant load error: " + std::string(exc.GetMessage()));
		}
	nm.unload(filename);
	//nm.remove(filename);
	OGRE_LOG("Poly count: " + Ogre::StringConverter::toString(totalPolys));
	if (result == false)
		mesh.setNull();
	return mesh;
	}

const Ogre::String PlantManager::createMaterial(const std::string &baseName, const int groupIndex,
        const P3DMaterialDef *material,
        bool &textured, const Ogre::String &resourceGroupName)
	{
	std::string matName;
	Ogre::MaterialPtr mat;
	Ogre::MaterialManager &mm = Ogre::MaterialManager::getSingleton();
	const char *texName = material->GetTexName(P3D_TEX_DIFFUSE);
	if(texName == NULL)
		{
		matName = baseName + "_" + Ogre::StringConverter::toString(groupIndex) +
		          ".material"; // kshepherd fixed toString
		textured = false;
		}
	else
		{
		matName = baseName + "_" + texName + ".material";
		textured = true;
		}
	if(!mm.resourceExists(matName))
		{
		float r, g, b;
#ifdef SUPPORT_NORMALS
		const char *normName = material->GetTexName(P3D_TEX_NORMAL_MAP);
		if(normName != NULL)
			{
			Ogre::MaterialPtr parent = mm.getByName("Examples/BumpMapping/MultiLight");
			mat = parent->clone(matName, true, resourceGroupName);
			Ogre::Technique *technique = mat->getTechnique(0);
			Ogre::Pass *perlight = technique->getPass("perlight");
			perlight->setLightingEnabled(true);
			Ogre::TextureUnitState *normalTexture = perlight->getTextureUnitState("normalmap");
			normalTexture->setTextureName(normName);
			if(texName != NULL)
				{
				Ogre::Pass *decal = technique->getPass("decal");
				if(material->IsTransparent())
					{
					decal->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
					decal->setCullingMode(Ogre::CULL_NONE);
					decal->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
					decal->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, (unsigned char)200);
					}
				Ogre::TextureUnitState *decalTexture = decal->getTextureUnitState("decalmap");
				decalTexture->setTextureName(texName);
				}
			}
		else
			{
#endif
			mat = mm.create(matName, resourceGroupName);
			mat->_notifyOrigin("OgrePlant");
			Ogre::Technique *technique = mat->getTechnique(0);
			Ogre::Pass *pass = technique->getPass(0);
			Ogre::TextureUnitState *textureUnit;
			if(texName != NULL)
				{
				textureUnit = pass->createTextureUnitState();
				textureUnit->setTextureName(texName);
				}
			material->GetColor(&r, &g, &b);
			mat->setDiffuse(r, g, b, 1.0f);
			mat->setAmbient(r, g, b);
			if(material->IsTransparent())
				{
				pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
				pass->setCullingMode(Ogre::CULL_NONE);
				pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
				pass->setAlphaRejectSettings(Ogre::CMPF_GREATER_EQUAL, (unsigned char)200);
				pass->setAlphaToCoverageEnabled(true);
				// Documentation seems to imply this is relevant only with single-channel
				// images, but manually written materials have the parameter.  Test each way.
				if(texName != NULL)
					textureUnit->setIsAlpha(true);
				}
#ifdef SUPPORT_NORMALS
			}
#endif
//    Ogre::MaterialSerializer materialSerializer;
//    materialSerializer.exportMaterial(mat, mat->getName());
		}
	// !!TODO!! P3D_TEX_AUX0 and P3D_TEX_AUX1
	texName = material->GetTexName(P3D_TEX_AUX0);
	if(texName != NULL)
		OGRE_LOG("Template has AUX0 texture");
	texName = material->GetTexName(P3D_TEX_AUX1);
	if(texName != NULL)
		OGRE_LOG("Template has AUX1 texture");
	return matName;
	}
