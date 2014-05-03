#ifndef FIRE_FOUNTAIN_H
#define FIRE_FOUNTAIN_H

#include "particle_system.h"

class FireFountain : public ParticleSystem {
	public:
		FireFountain();
		~FireFountain();
		void computeParticles();

	private:
		void createNewParticles();
};

#endif /* fountain.h */