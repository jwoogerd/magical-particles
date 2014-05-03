#ifndef REMCO_FOUNTAIN_H
#define REMCO_FOUNTAIN_H

#include "particle_system.h"

class RemcoFountain : public ParticleSystem {
	public:
		RemcoFountain();
		~RemcoFountain();
		void computeParticles();
        void setHuman(std::string imagefile, std::string fragfile);

	private:
		void createNewParticles();
};

#endif /* fountain.h */