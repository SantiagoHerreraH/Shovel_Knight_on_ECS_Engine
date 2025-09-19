
#include "../../../include/Structs/FShape.h"
#include "../../../include/Systems/TransformSystems/UMatrixTransformer.h"

namespace Pillar {

	class UApplyMatrixToShape2D {

	public:
		UApplyMatrixToShape2D(URegistry& registry);
		void Update();
	private:
		UComponentIterator<FShape2Df> m_Shapes;
		UComponentIterator<FBaseShape2Df> m_BaseShapes;
		UComponentIterator<FMatrix4x4f> m_Matrices;
	};

	class UCalculateShapeCenters {

	public:
		UCalculateShapeCenters(URegistry& registry);
		void Update();
	private:
		UComponentIterator<FShape2Df> m_Shapes;

	};

	class UShapeTransformer2D {

	public:
		UShapeTransformer2D(URegistry& registry);

		void Update();

	private:

		UResetMatrices				m_ResetMatrices;
		UApplyLocalTransform2DToMatrix	m_ApplyTransformToMatrix;
		UApplyMatrixToShape2D		m_ApplyMatrixToShape;

	};
}