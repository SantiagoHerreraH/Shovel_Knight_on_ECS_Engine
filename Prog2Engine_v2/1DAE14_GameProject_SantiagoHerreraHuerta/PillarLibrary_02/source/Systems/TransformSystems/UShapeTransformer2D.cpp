#include "../../../include/Systems/TransformSystems/UShapeTransformer2D.h"
#include "../../../include/Structs/FMesh.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"
#include "../../../include/Systems/MathSystems/SGeometry.h"

namespace Pillar {

	UShapeTransformer2D::UShapeTransformer2D(URegistry& registry) :
		m_ResetMatrices(registry),
		m_ApplyTransformToMatrix(registry),
		m_ApplyMatrixToShape(registry)
	{
	}

	void UShapeTransformer2D::Update()
	{
		m_ApplyTransformToMatrix.Update();
		m_ApplyMatrixToShape.Update();
		m_ResetMatrices.Reset();
	}

	UApplyMatrixToShape2D::UApplyMatrixToShape2D(URegistry& registry)
	{
		FShape2Df shape{};
		FBaseShape2Df baseShape{};
		FMatrix4x4f matrix{};
		FMesh2Df mesh{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(shape, baseShape, matrix);
		filter.SetExcludingFilter(mesh);

		registry.GetComponents(m_Shapes, filter);
		registry.GetComponents(m_BaseShapes, filter);
		registry.GetComponents(m_Matrices, filter);
	}

	void UApplyMatrixToShape2D::Update()
	{
		for (size_t archIdx = 0; archIdx < m_Shapes.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Shapes.NumOfComponents(archIdx); compIdx++)
			{
				for (size_t vrtxIdx = 0; vrtxIdx < m_BaseShapes.At(archIdx, compIdx).Shape.size(); vrtxIdx++)
				{
					//Matrices are Not a number
					SMatrix::Multiply(
						m_Matrices.At(archIdx, compIdx),
						m_BaseShapes.At(archIdx, compIdx).Shape[vrtxIdx],
						m_Shapes.At(archIdx, compIdx).Shape[vrtxIdx]);
				}
			}
		}
	}
	UCalculateShapeCenters::UCalculateShapeCenters(URegistry& registry)
	{
		registry.GetComponents(m_Shapes);
	}
	void UCalculateShapeCenters::Update()
	{
		for (size_t archIdx = 0; archIdx < m_Shapes.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Shapes.NumOfComponents(archIdx); compIdx++)
			{
				m_Shapes.At(archIdx, compIdx).Center = SGeometry::Center(m_Shapes.At(archIdx, compIdx));
			}
		}
	}
}