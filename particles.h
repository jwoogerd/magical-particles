#ifndef PARTICLES_H
#define PARTICLES_H

#include "Algebra.h"
#define DECAY 0.001

struct Color {
	float r, g, b, a;
};

struct Speed {
	float xspeed, yspeed, zspeed;
};

class Particle {
	public:		
		Particle();
		Particle(float lifetime, float decay, Color color, 
				Point pos, float *speed, bool active);
		~Particle();

		virtual void EvolveParticle();

		float lifetime; /* total lifetime of the particle */
		Color color;    /* color values of the particle */
		Point pos;		/* particle's position in 3-space */
		Speed speed;    /* particle's speed */
		bool active;    /* is particle active or not? */
};

#endif  /* particles.h */