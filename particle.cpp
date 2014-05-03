#include "particle.h"

Particle::Particle()
{
	lifetime = -1.0f;
	cameradistance = -1.0f;
	color.r = 0;
	color.g = 0;
	color.b = 0;
	color.a = 1;
	speed = Vector();
	pos = Point();
}

Particle::Particle(float lifetime, Color color, Point pos, Vector speed)
{
	this->lifetime = lifetime;
	this->color = color;
	this->speed = speed;
	this->pos = pos;
}

Particle::~Particle() {}