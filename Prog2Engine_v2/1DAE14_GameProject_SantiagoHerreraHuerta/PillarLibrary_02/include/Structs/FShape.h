#pragma once

#include "../../include/Structs/FVector.h"

namespace Pillar {

#pragma region Shape

	using DShape2Df = std::vector<FPoint2f>;
	using D_Shape2D_Container = std::vector<DShape2Df>;

	using DShape3Df = std::vector<FPoint3f>;
	using D_Shape3Df_Container = std::vector<DShape3Df>;

	struct FShape2Df {

		DShape2Df Shape;
		FPoint2f Center;
	};

	struct FBaseShape2Df {

		DShape2Df Shape;
		FPoint2f Center;
	};

#pragma endregion

#pragma region Line

	struct FLine2f {
		FPoint2f Start;
		FPoint2f End;
	};

	struct FLine2i {
		FPoint2i Start;
		FPoint2i End;
	};

	struct FLine2 {
		FPoint2 Start;
		FPoint2 End;
	};

#pragma endregion

#pragma region Circle

	struct FCirclef {

		FPoint2f Center;

		float Radius;

	};

	struct FCirclei {

		FPoint2i Center;

		int Radius;

	};

	struct FCircle {

		FPoint2 Center;

		double Radius;

	};

#pragma endregion

#pragma region Rectangle

	struct FRectf {

		FRectf() {};
		explicit FRectf(float left, float top, float width, float height) : Left(left), Top(top), Width(width), Height(height) {};
		float Left{0.f};
		float Top{0.f};
		float Width{1.f};
		float Height{1.f};

	};

	struct FRecti {

		int Left;
		int Top;
		int Width;
		int Height;

	};

	struct FRect {

		double Left;
		double Top;
		double Width;
		double Height;

	};

	struct FRectCollider {
		FRectCollider() {}
		explicit FRectCollider(float left, float top, float width, float height, bool optimize = true) : 
			OriginalRect(left, top, width, height), 
			CurrentRect(left, top, width, height),
			Optimize(optimize){};
		FRectf OriginalRect{};
		FRectf CurrentRect{};
		bool Optimize{ true };
	};

	struct FStaticColliderFlag {
		bool IsStatic{true};
	};

#pragma endregion

}