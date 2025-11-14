#include "CameraSceneNode.h"
#include "../Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Sound/SoundManager.h"
#include "PrimeEngine/ParticleSystems/ParticleEmitter.h"


#define Z_ONLY_CAM_BIAS 0.0f
namespace PE {
namespace Components {

PE_IMPLEMENT_CLASS1(CameraSceneNode, SceneNode);

CameraSceneNode::CameraSceneNode(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself) : SceneNode(context, arena, hMyself)
{
	m_near = 0.05f;
	m_far = 2000.0f;
	m_pContext->getSoundManager()->createListener(hMyself);
	m_pContext->getParticleEmitter()->createCamera(hMyself);
}
void CameraSceneNode::addDefaultComponents()
{
	Component::addDefaultComponents();
	PE_REGISTER_EVENT_HANDLER(Events::Event_CALCULATE_TRANSFORMATIONS, CameraSceneNode::do_CALCULATE_TRANSFORMATIONS);
}

void CameraSceneNode::do_CALCULATE_TRANSFORMATIONS(Events::Event *pEvt)
{
	Handle hParentSN = getFirstParentByType<SceneNode>();
	if (hParentSN.isValid())
	{
		Matrix4x4 parentTransform = hParentSN.getObject<PE::Components::SceneNode>()->m_worldTransform;
		m_worldTransform = parentTransform * m_base;
	}
	
	Matrix4x4 &mref_worldTransform = m_worldTransform;

	pos = Vector3(mref_worldTransform.m[0][3], mref_worldTransform.m[1][3], mref_worldTransform.m[2][3]);
	Vector3 n = Vector3(mref_worldTransform.m[0][2], mref_worldTransform.m[1][2], mref_worldTransform.m[2][2]);
	Vector3 target = pos + n;
	Vector3 up = Vector3(mref_worldTransform.m[0][1], mref_worldTransform.m[1][1], mref_worldTransform.m[2][1]);
	Vector3 left = n.crossProduct(up);

	m_worldToViewTransform = CameraOps::CreateViewMatrix(pos, target, up);

	m_worldTransform2 = mref_worldTransform;

	m_worldTransform2.moveForward(Z_ONLY_CAM_BIAS);

	Vector3 pos2 = Vector3(m_worldTransform2.m[0][3], m_worldTransform2.m[1][3], m_worldTransform2.m[2][3]);
	Vector3 n2 = Vector3(m_worldTransform2.m[0][2], m_worldTransform2.m[1][2], m_worldTransform2.m[2][2]);
	Vector3 target2 = pos2 + n2;
	Vector3 up2 = Vector3(m_worldTransform2.m[0][1], m_worldTransform2.m[1][1], m_worldTransform2.m[2][1]);
	

	m_worldToViewTransform2 = CameraOps::CreateViewMatrix(pos2, target2, up2);
    
    PrimitiveTypes::Float32 aspect = (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getWidth()) / (PrimitiveTypes::Float32)(m_pContext->getGPUScreen()->getHeight());
    
    PrimitiveTypes::Float32 verticalFov = 0.33f * PrimitiveTypes::Constants::c_Pi_F32;
    if (aspect < 1.0f)
    {
        //ios portrait view
        static PrimitiveTypes::Float32 factor = 0.5f;
        verticalFov *= factor;
    }

	m_viewToProjectedTransform = CameraOps::CreateProjectionMatrix(verticalFov, aspect,	m_near, m_far);
	
	//Camera Frustum
	float fnear = 0.05f;
	float ffar = 2000.0f;
	
	farCenter = pos + n * ffar;
	nearCenter = pos + n * fnear;

	float farHalfHeight = tanf(verticalFov) * ffar;
	//farHalfHeight = farHalfHeight / 5;

	float farHalfWidth = farHalfHeight * aspect;
	//farHalfWidth = farHalfWidth / 5;

	Vector3 farTopLeft = m_worldTransform * Vector3 (-farHalfWidth,farHalfHeight, ffar);
	Vector3 farTopRight = m_worldTransform * Vector3(farHalfWidth, farHalfHeight, ffar);
	Vector3 farBotLeft = m_worldTransform * Vector3(-farHalfWidth, -farHalfHeight, ffar);
	Vector3 farBotRight = m_worldTransform * Vector3(farHalfWidth, -farHalfHeight, ffar);


    Vector3 p0 = pos;
    Vector3 p1 = farBotLeft;
	Vector3 p2 = farTopLeft;
	Vector3 CrossDiff = (p1 - p0).crossProduct(p2-p1);
	
	LeftPlane = CrossDiff;

	p0 = pos;
	p1 = farTopLeft;
	p2 = farTopRight;
	CrossDiff = (p1 - p0).crossProduct(p2 - p1);

	TopPlane = CrossDiff;
	
	p0 = pos;
	p1 = farTopRight;
	p2 = farBotRight;
	CrossDiff = (p1 - p0).crossProduct(p2 - p1);
	
	RightPlane = CrossDiff;

	p0 = pos;
	p1 = farBotRight;
	p2 = farBotLeft;
	CrossDiff = (p1 - p0).crossProduct(p2 - p1);
	
	BotPlane = CrossDiff;

	n.normalize();
	NearPlane =  n;
	FarPlane = -n;

	

	SceneNode::do_CALCULATE_TRANSFORMATIONS(pEvt);

}

}; // namespace Components
}; // namespace PE
