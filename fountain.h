#ifndef FOUNTAIN_H
#define FOUNTAIN_H

#include <string>
#include "particle_system.h"

class Fountain : public ParticleSystem {
	public:
		Fountain(particle_type system_type);
		Fountain(particle_type system_type, int particle_size, 
				 int max_particles, float spread, std::string *files);
		~Fountain();
		void computeParticles();
		void setHuman(std::string imagefile, std::string fragfile);

	private:
		void createNewParticles();
};

#endif /* fountain.h */