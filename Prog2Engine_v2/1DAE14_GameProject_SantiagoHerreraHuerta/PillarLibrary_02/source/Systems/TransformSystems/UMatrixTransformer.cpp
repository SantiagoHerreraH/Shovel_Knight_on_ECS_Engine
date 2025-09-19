
#include "../../../include/Systems/TransformSystems/UMatrixTransformer.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"

namespace Pillar {

	UApplyLocalTransform2DToMatrix::UApplyLocalTransform2DToMatrix(URegistry& registry)
	{
		FLocalTransform2D transform{};
		FMatrix4x4f matrix{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(transform, matrix);

		registry.GetComponents(m_Transforms2D, filter);
		registry.GetComponents(m_Matrices, filter);
	}

	void UApplyLocalTransform2DToMatrix::Update()
	{
		for (size_t archIdx = 0; archIdx < m_Transforms2D.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Transforms2D.NumOfActivatedComponents(archIdx); compIdx++)
			{
				SMatrix::ApplyTransform(m_Matrices.At(archIdx, compIdx), m_Transforms2D.At(archIdx, compIdx).Transform);
			}
		}
	}

	UResetMatrices::UResetMatrices(URegistry& registry)
	{
		registry.GetComponents(m_Matrices);
	}

	void UResetMatrices::Reset()
	{
		for (size_t archIdx = 0; archIdx < m_Matrices.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Matrices.NumOfActivatedComponents(archIdx); compIdx++)
			{
				SMatrix::MakeIdentity(m_Matrices.At(archIdx, compIdx));
			}
		}
	}


}