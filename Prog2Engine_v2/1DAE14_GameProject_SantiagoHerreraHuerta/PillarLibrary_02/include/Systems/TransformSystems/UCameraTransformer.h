#pragma once

#include "../../../include/Structs/FCamera.h"
#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FMatrix.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {

	class UApplyCameraTransformToWorld {

	public:
		UApplyCameraTransformToWorld(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FCamera> m_Camera;
		UComponentIterator<URealTransform2D> m_CameraTransform;
		UComponentIterator<FMatrix4x4f> m_Matrices;
	};


}