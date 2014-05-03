#ifndef FOUNTAIN_H
#define FOUNTAIN_H

#include "particle_system.h"

class Fountain : public ParticleSystem {
	public:
		Fountain(particle_type system_type);
		~Fountain();
		void computeParticles();

	private:
		void createNewParticles();
};

#endif /* fountain.h */