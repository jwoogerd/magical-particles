#ifndef PARTICLE_H
#define PARTICLE_H

#include "Algebra.h"

struct Color {
	union {
		struct {
			float r, g, b, a;
		};
		float channels[4];
	};
};

class Particle {
	public:		
		Particle();
		Particle(float lifetime, Color color, Point pos, Vector speed);
		~Particle();

		float lifetime; 	  /* total lifetime of the particle */
		Color color;          /* color values of the particle */
		Point pos;		      /* particle's position in 3-space */
		Vector speed;         /* particle's speed */
		float size;           /* particles size */
		float cameradistance; /* squared distance to the camera. if dead: -1.0f */

		bool operator<(const Particle& that) const {
			return this->cameradistance > that.cameradistance;
		}
};

#endif  /* particle.h */