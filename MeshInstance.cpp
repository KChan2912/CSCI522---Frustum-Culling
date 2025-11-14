#define NOMINMAX
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes

// Inter-Engine includes
#include "PrimeEngine/FileSystem/FileReader.h"
#include "PrimeEngine/APIAbstraction/GPUMaterial/GPUMaterialSet.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "PrimeEngine/APIAbstraction/Texture/Texture.h"
#include "PrimeEngine/APIAbstraction/Effect/EffectManager.h"
#include "PrimeEngine/APIAbstraction/GPUBuffers/VertexBufferGPUManager.h"
#include "PrimeEngine/Scene/Skeleton.h"
#include "PrimeEngine/Scene/DebugRenderer.h"
#include "DefaultAnimationSM.h"
#include "Light.h"

#include "PrimeEngine/GameObjectModel/Camera.h"


// Sibling/Children includes
#include "MeshInstance.h"
#include "MeshManager.h"
#include "SceneNode.h"
#include "CameraManager.h"
#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
namespace PE {
namespace Components{

PE_IMPLEMENT_CLASS1(MeshInstance, Component);

MeshInstance::MeshInstance(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself)
: Component(context, arena, hMyself)
, m_culledOut(false)
, aabbCorners(context, arena, 8)
{
	
}

void MeshInstance::addDefaultComponents()
{
	Component::addDefaultComponents();
	
}

void MeshInstance::DrawBB(Matrix4x4 WT)
{
	if (aabbCorners.m_size > 0)
	{
		Matrix4x4 base = WT;

		Vector3 target = base * aabbCorners[2];
		Vector3 pos = base * aabbCorners[0];
		Vector3 color(1.0f, 1.0f, 0);
		Vector3 linepts[] = { pos, color, target, color };

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[4];
		pos = base * aabbCorners[0];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[3];
		pos = base * aabbCorners[0];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[6];
		pos = base * aabbCorners[2];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[7];
		pos = base * aabbCorners[2];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[7];
		pos = base * aabbCorners[4];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[5];
		pos = base * aabbCorners[4];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[6];
		pos = base * aabbCorners[3];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[5];
		pos = base * aabbCorners[3];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[1];
		pos = base * aabbCorners[6];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[1];
		pos = base * aabbCorners[5];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

		target = base * aabbCorners[1];
		pos = base * aabbCorners[7];

		linepts[0] = pos;
		linepts[2] = target;

		DebugRenderer::Instance()->createLineMesh(false, base, &linepts[0].m_x, 2, 0);

	}

}

void MeshInstance::initFromFile(const char *assetName, const char *assetPackage,
		int &threadOwnershipMask)
{
	Handle h = m_pContext->getMeshManager()->getAsset(assetName, assetPackage, threadOwnershipMask);
	
	initFromRegisteredAsset(h);

	Mesh* pMesh = m_hAsset.getObject<Mesh>();
	for (int i = 0; i < pMesh->aabb.m_capacity; i++) {
		aabbCorners.add(Vector3(pMesh->aabb[i]));
	}
	
}

bool MeshInstance::hasSkinWeights()
{
	Mesh *pMesh = m_hAsset.getObject<Mesh>();
	return pMesh->m_hSkinWeightsCPU.isValid();
}

void MeshInstance::initFromRegisteredAsset(const PE::Handle &h)
{
	m_hAsset = h;
	//add this instance as child to Mesh so that skin knows what to draw
	static int allowedEvts[] = {0};
	m_hAsset.getObject<Component>()->addComponent(m_hMyself, &allowedEvts[0]);
}


}; // namespace Components
}; // namespace PE
