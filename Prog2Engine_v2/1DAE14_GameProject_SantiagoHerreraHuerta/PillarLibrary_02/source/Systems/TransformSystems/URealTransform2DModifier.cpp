
#include "../../../include/Systems/TransformSystems/URealTransform2DModifier.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {
	void URealTransform2DModifier::Reset(URealTransform2D* worldTransform)
	{
		worldTransform->m_RealWorldTransform.Position	= { 0,0 };
		worldTransform->m_RealWorldTransform.RotationZ	= 0;
		worldTransform->m_RealWorldTransform.Scale		= { 1,1 };
		worldTransform->m_ScreenTransform.Position		= { 0,0 };
		worldTransform->m_ScreenTransform.RotationZ		= 0;
		worldTransform->m_ScreenTransform.Scale			= { 1,1 };
	}
	FTransform2D& URealTransform2DModifier::GetRealWorldTransform(URealTransform2D* worldTransform)
	{
		return worldTransform->m_RealWorldTransform;
	}
	FTransform2D& URealTransform2DModifier::GetScreenTransform(URealTransform2D* worldTransform)
	{
		return worldTransform->m_ScreenTransform;
	}
}