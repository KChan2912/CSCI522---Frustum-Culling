// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "MeshManager.h"
// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/../../GlobalConfig/GlobalConfig.h"

#include "PrimeEngine/Geometry/SkeletonCPU/SkeletonCPU.h"

#include "PrimeEngine/Scene/RootSceneNode.h"

#include "Light.h"

// Sibling/Children includes

#include "MeshInstance.h"
#include "Skeleton.h"
#include "SceneNode.h"
#include "DrawList.h"
#include "SH_DRAW.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"

namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshManager, Component);
MeshManager::MeshManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
	: Component(context, arena, hMyself)
	, m_assets(context, arena, 256)
{
}

PE::Handle MeshManager::getAsset(const char *asset, const char *package, int &threadOwnershipMask)
{
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "%s/%s", package, asset);
	
	int index = m_assets.findIndex(key);
	if (index != -1)
	{
		return m_assets.m_pairs[index].m_value;
	}
	Handle h;

	if (StringOps::endswith(asset, "skela"))
	{
		PE::Handle hSkeleton("Skeleton", sizeof(Skeleton));
		Skeleton *pSkeleton = new(hSkeleton) Skeleton(*m_pContext, m_arena, hSkeleton);
		pSkeleton->addDefaultComponents();

		pSkeleton->initFromFiles(asset, package, threadOwnershipMask);
		h = hSkeleton;
	}
	else if (StringOps::endswith(asset, "mesha"))
	{
		MeshCPU mcpu(*m_pContext, m_arena);
		mcpu.ReadMesh(asset, package, "");

		
		
		//std::cout << "THIS IS WITHIN MESHMANAGER CPP!!!!!!!!" << std::endl << m_minX << std::endl;
		PE::Handle hMesh("Mesh", sizeof(Mesh));
		Mesh *pMesh = new(hMesh) Mesh(*m_pContext, m_arena, hMesh);
		pMesh->addDefaultComponents();
		

		pMesh->loadFromMeshCPU_needsRC(mcpu, threadOwnershipMask);

#if PE_API_IS_D3D11
		// todo: work out how lods will work
		//scpu.buildLod();
#endif
        // generate collision volume here. or you could generate it in MeshCPU::ReadMesh()
		Array<float> m_position_values = mcpu.m_hPositionBufferCPU.getObject<PositionBufferCPU>()->m_values;
		m_minX = m_position_values[0];
		m_minY = m_position_values[1];
		m_minZ = m_position_values[2];
		m_maxX = m_position_values[0];
		m_maxY = m_position_values[1];
		m_maxZ = m_position_values[2];
		for (int i = 3; i < m_position_values.m_size; i++)
		{
			if (i % 3 == 0)
			{
				if (m_position_values[i] < m_minX)
				{
					m_minX = m_position_values[i];
				}
				if (m_position_values[i] > m_maxX)
				{
					m_maxX = m_position_values[i];
				}
			}
			else if (i % 3 == 1)
			{
				if (m_position_values[i] < m_minY)
				{
					m_minY = m_position_values[i];
				}
				if (m_position_values[i] > m_maxY)
				{
					m_maxY = m_position_values[i];
				}
			}
			else if (i % 3 == 2)
			{
				if (m_position_values[i] < m_minZ)
				{
					m_minZ = m_position_values[i];
				}
				if (m_position_values[i] > m_maxZ)
				{
					m_maxZ = m_position_values[i];
				}
			}
		}
		pMesh->aabb.add(Vector3(m_minX, m_minY, m_minZ));//1[0]
		pMesh->aabb.add(Vector3(m_maxX, m_maxY, m_maxZ));//8[1]
		pMesh->aabb.add(Vector3(m_minX, m_maxY, m_minZ));//2[2]
		pMesh->aabb.add(Vector3(m_minX, m_minY, m_maxZ));//4[3]
		pMesh->aabb.add(Vector3(m_maxX, m_minY, m_minZ));//3[4]
		pMesh->aabb.add(Vector3(m_maxX, m_minY, m_maxZ));//6[5]
		pMesh->aabb.add(Vector3(m_minX, m_maxY, m_maxZ));//5[6]
		pMesh->aabb.add(Vector3(m_maxX, m_maxY, m_minZ));//7[7]
        pMesh->m_performBoundingVolumeCulling = true; // will now perform tests for this mesh

		h = hMesh;
	}


	PEASSERT(h.isValid(), "Something must need to be loaded here");

	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
	return h;
}

void MeshManager::registerAsset(const PE::Handle &h)
{
	static int uniqueId = 0;
	++uniqueId;
	char key[StrTPair<Handle>::StrSize];
	sprintf(key, "__generated_%d", uniqueId);
	
	int index = m_assets.findIndex(key);
	PEASSERT(index == -1, "Generated meshes have to be unique");
	
	RootSceneNode::Instance()->addComponent(h);
	m_assets.add(key, h);
}

}; // namespace Components
}; // namespace PE
