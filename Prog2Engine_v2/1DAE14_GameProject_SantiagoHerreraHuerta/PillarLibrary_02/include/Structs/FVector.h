#pragma once

#include <vector>

namespace Pillar {

	/*

		2D

	*/


	struct FPoint2
	{
		double X{};
		double Y{};
	};

	struct FPoint2i
	{
		int X{};
		int Y{};
	};

	struct FPoint2f
	{
		float X{};
		float Y{};
	};

	


	/*

		3D

	*/

	template<typename T>
	struct FPoint3D {
	
		T X{}, Y{}, Z{};
	};

	using FPoint3i = FPoint3D<int>;
	using FPoint3  = FPoint3D<double>;
	using FPoint3f = FPoint3D<float>;

	

	/*

		4D

	*/

	struct FPoint4 {
		double X{}, Y{}, Z{}, W{};
	};

	struct FPoint4f {
		float X{}, Y{}, Z{}, W{};
	};

	/*

		RELATED OTHERS

	*/


	struct FVector {

		double AngleInDegrees{};
		double Length{};
	};

	struct FPoint2AndLength {

		FPoint2f Point{};
		double Length{};
	};

	struct FRotation3 {
		double X{};
		double Y{};
		double Z{};
	};

	struct FRotation3f {
		float X{};
		float Y{};
		float Z{};
	};

}