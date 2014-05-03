#include "remcofountain.h"
#include <GL/glui.h>
#include <stdlib.h>


RemcoFountain::RemcoFountain()
{
    system_type = IMAGE;
    max_particles = 1000;
    vertex_shader = "shaders/remcoparticle.vert"; 
    fragment_shader = "shaders/remcoparticle.frag"; 
    texture_file = "shaders/remcochang.ppm";
    spread = 5.0f; 
    particle_size = 30.0;
    radius = 1.5f;
    main_direction = Vector(0.0, 12.0, 0.0);
    gravity = Vector(0.0f, -9.81f, 0.0f);
}

RemcoFountain::~RemcoFountain(){}

void RemcoFountain::createNewParticles()
{
    int newparticles = (elapsed * max_particles)/lifetime;
    if (newparticles > max_particles / 2.0) { 
        newparticles = max_particles / 2.0;
    }
    for (int i = 0; i < newparticles; i++) {
        int particle_index = findDeadParticle();
        Vector rand_direction = getRandVector();

        particles[particle_index].lifetime = lifetime;
        particles[particle_index].pos = position;
        particles[particle_index].speed = main_direction + rand_direction * spread;
        particles[particle_index].size = (rand() % particle_size) / 200.0f + 0.01f;
    }
}

void RemcoFountain::computeParticles()
{
    active_particles = 0;

    createNewParticles();
    for (int i = 0; i < max_particles; i++) {
        Particle *p = &particles[i];

        if (p->lifetime > 0.0f) {
            p->lifetime -= elapsed;
            if (p->lifetime > 0.0f) {
                p->speed = p->speed + (gravity * elapsed);
                p->pos = p->pos + (p->speed * elapsed / 10);
                p->cameradistance = length((p->pos - camera_position));
                setGPUBuffers(p, active_particles);
            } else {
                p->cameradistance = -1.0f; /* particle has just died */
            }
            active_particles++;
        }
    }
}


void RemcoFountain::setHuman(std::string imagefile, std::string fragfile) 
{
    texture_file = imagefile;
    fragment_shader = fragfile;
}
