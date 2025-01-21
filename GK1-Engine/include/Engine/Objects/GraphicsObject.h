#pragma once
#include "Engine/Transform.h"

class GraphicsObject : public Transform
{
public:
	virtual ~GraphicsObject() = default;
	virtual void Draw() {}
};