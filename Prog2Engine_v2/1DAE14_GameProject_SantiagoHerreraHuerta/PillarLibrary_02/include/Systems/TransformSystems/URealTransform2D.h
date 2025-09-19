#pragma once

#include "../../../include/Structs/FTransform.h"
#include "URealTransform2DModifier.h"

namespace Pillar {


	class URealTransform2D {

	public:

		const FTransform2D& GetConstRealWorldTransform() const { return m_RealWorldTransform; }
		const FTransform2D& GetConstScreenTransform() const { return m_ScreenTransform; }

	private:

		FTransform2D m_RealWorldTransform{};
		FTransform2D m_ScreenTransform{};
		
		friend class URealTransform2DModifier;
	};

}




