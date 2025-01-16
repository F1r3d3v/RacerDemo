#pragma once

class GraphicsObject
{
public:
	virtual ~GraphicsObject() = default;
	virtual void Draw() = 0;
};