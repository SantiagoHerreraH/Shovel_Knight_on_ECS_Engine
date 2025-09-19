
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FMatrix.h"
#include "../../../include/Core/URegistry.h"

namespace Pillar {

	class UResetMatrices {

	public:

		UResetMatrices(URegistry& registry);
		void Reset();

	private:
		UComponentIterator<FMatrix4x4f> m_Matrices;

	};

	class UApplyLocalTransform2DToMatrix {

	public:
		UApplyLocalTransform2DToMatrix(URegistry& registry);
		void Update();
	private:
		UComponentIterator<FLocalTransform2D> m_Transforms2D;
		UComponentIterator<FMatrix4x4f> m_Matrices;
	};

}