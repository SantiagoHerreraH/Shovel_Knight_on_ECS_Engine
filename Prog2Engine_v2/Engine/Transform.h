#pragma once
#include "base.h"
#include "Vector2f.h"

struct Transform
{
public:
	void ApplyTransformation() const
	{
		glPushMatrix();
		glTranslatef(Position.x, Position.y, 0);
		glRotatef(RotationZ, 0, 0, 1);
		glRotatef(RotationY, 0, 1, 0);
		glRotatef(RotationX, 1, 0, 0);
		glScalef(Scale.x, Scale.y, 1);
	}

	void ResetTransformation() const
	{
		glPopMatrix();
	}

	Vector2f Position{};
	float RotationX{};
	float RotationY{};
	float RotationZ{};
	Vector2f Scale{ 1.f, 1.f };
};
