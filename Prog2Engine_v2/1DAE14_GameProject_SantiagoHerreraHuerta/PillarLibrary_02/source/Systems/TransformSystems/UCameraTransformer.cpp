
#include "../../../include/Systems/TransformSystems/UCameraTransformer.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {

	UApplyCameraTransformToWorld::UApplyCameraTransformToWorld(URegistry& registry)
	{

		FCamera camera{};
		UComponentFilter filterMatrices{};
		filterMatrices.SetExcludingFilter(camera);
		registry.GetComponents(m_Matrices);

		
		URealTransform2D worldTransform{};
		UComponentFilter filter{};
		filter.SetIncludingFilter(camera, worldTransform);
		registry.GetComponents(m_CameraTransform, filter);
		registry.GetComponents(m_Camera, filter);
	}


	void UApplyCameraTransformToWorld::Update()
	{
		FTransform2D inverseTransform{};
		inverseTransform.Position.X = -(m_CameraTransform.At(0, 0).GetConstRealWorldTransform().Position.X - m_Camera.At(0,0).ScreenWidth / 2.f);
		inverseTransform.Position.Y = -(m_CameraTransform.At(0, 0).GetConstRealWorldTransform().Position.Y - m_Camera.At(0,0).ScreenHeight / 2.f);
		inverseTransform.RotationZ = -m_CameraTransform.At(0,0).GetConstRealWorldTransform().RotationZ;

		for (size_t archIdx = 0; archIdx < m_Matrices.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Matrices.NumOfActivatedComponents(archIdx); compIdx++)
			{
				SMatrix::ApplyTransform(m_Matrices.At(archIdx, compIdx), inverseTransform);
			}
		}
	}

}