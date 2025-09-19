#pragma once

#include "../../../include/Structs/FTransform.h"

namespace Pillar {

	class URealTransform2D;

	class URealTransform2DModifier {

	public:
		void Reset(URealTransform2D* worldTransform);
		FTransform2D& GetRealWorldTransform(URealTransform2D* worldTransform);
		FTransform2D& GetScreenTransform(URealTransform2D* worldTransform);
	};
}