#pragma once


#include <vector>
#include <array>
#include "FVector.h"
#include "FColor.h"

namespace Pillar {

	struct FTriangle2Di {
		FPoint2i Vertices[3]{};
	};

	struct FTriangle2D {
		FPoint2 Vertices[3]{};
	};

	struct FTriangle2f {
		FPoint2f Vertices[3]{};
	};

	using DMesh2Df = std::vector<FTriangle2f>;
	using D_Mesh2D_Container = std::vector<DMesh2Df>;

	struct FMesh2Df {
		DMesh2Df Triangles;
	};

	struct FBaseMesh2Df {
		DMesh2Df Triangles;
	};

	/*

		3D

	*/

	struct FTriangle3D {
		FPoint3 Vertices[3]{};
	};

	struct FTriangle3Df {
		FPoint3f Vertices[3]{};
	};
	struct FTriangle3Di {
		FPoint3i Vertices[3]{};
	};

	struct FTriangle4D {
		FPoint4 Vertices[3]{};
	};

	using DMesh = std::vector<FTriangle3Df>;

	using D_Mesh_Container = std::vector<DMesh>;

	struct FCompleteTriangle3Df {
		FTriangle3Df Triangle{};
		FPoint3f NormalizedNormal{};
		FColor4i Color{};
	};

	using DCompleteMesh = std::vector<FCompleteTriangle3Df>;
	using D_CompleteMesh_Container = std::vector<DCompleteMesh>;



	/*
		The "standard" way of doing matrix multiplication is the following:

			X		Y		Z		W

			*		*		*		*
			|		|		|		|
			v		v		v		v

			a	+	b	+	c	+	d	= X
			a	+	b	+	c	+	d	= Y
			a	+	b	+	c	+	d	= Z
			a	+	b	+	c	+	d	= W


			You are doing multiplication this way
			Arrays are contiguos in memory, so the are actually just giant rows.
			However, lets visualize it like this for clarity:

			 X			 Y			 Z			 W

			 *			 *			 *			 *
			 |			 |			 |			 |
			 v			 v			 v			 v

			[0][0]	+	[0][1]	+	[0][2]	+	[0][3]	= X
			[1][0]	+	[1][1]	+	[1][2]	+	[1][3]	= Y
			[2][0]	+	[2][1]	+	[2][2]	+	[2][3]	= Z
			[3][0]	+	[3][1]	+	[3][2]	+	[3][3]	= W

	*/

}