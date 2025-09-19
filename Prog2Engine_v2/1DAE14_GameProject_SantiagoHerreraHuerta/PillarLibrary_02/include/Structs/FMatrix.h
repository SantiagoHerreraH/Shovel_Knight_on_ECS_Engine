#pragma once

#include <array>

namespace Pillar {

	using DMatrix4x4f = std::array<std::array<float, 4>, 4>;

	using DMatrix3x3f = std::array<std::array<float, 3>, 3>;

	using DMatrix2x2f = std::array<std::array<float, 2>, 2>;

	struct FMatrix4x4f {

		FMatrix4x4f() {
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 4; j++)
				{
					if (i == j)
					{
						Values[i][j] = 1;
					}
					else
					{
						Values[i][j] = 0;
					}
				}

			}
		};

		DMatrix4x4f Values;
	};


	//IMPORTANT :
	/*
	since calculating the inverse of 3x3 matrices is too time consuming, in the future, 
	if you have matrices that transform the vertices of an object and that transform the shape before 
	the TRS transformation, those transformations will have to be stored in different matrices from the TRS matrices,
	so getting the inverse of the TRS matrices is easier and faster
	
	*/
}