#include "fountain.h"
#include <GL/glui.h>
#include <stdlib.h>

Fountain::Fountain(particle_type system_type)
{
    this->system_type = system_type;
    max_particles = 10000;
    vertex_shader = "shaders/colorparticle.vert"; 
    fragment_shader = "shaders/colorparticle.frag"; 
    texture_file = "shaders/particle.dds";
    spread = 1.0f; 
    particle_size = 10;
    radius = 1.5f;
    main_direction = Vector(0.0, 12.0, 0.0);
    gravity = Vector(0.0f, -9.81f, 0.0f);
}

Fountain::~Fountain(){}

void Fountain::createNewParticles()
{
    int newparticles = (elapsed * max_particles)/lifetime;
    if (newparticles > max_particles / 2.0) { 
        newparticles = max_particles / 2.0;
    }
    for (int i = 0; i < newparticles; i++) {
        int particle_index = findDeadParticle();
        particles[particle_index].lifetime = lifetime;
        particles[particle_index].pos = position;

        Vector rand_direction = getRandVector();

        particles[particle_index].speed = main_direction + rand_direction * spread;
        /* TODO: generate random colors */
        particles[particle_index].color.r = rand() / (float)RAND_MAX;
        particles[particle_index].color.g = rand() / (float)RAND_MAX;
        particles[particle_index].color.b = rand() / (float)RAND_MAX;
        particles[particle_index].color.a = rand() / (float)RAND_MAX; 

        particles[particle_index].size = (rand() % particle_size) / 200.0f + 0.01f;
    }
}

void Fountain::computeParticles()
{
    active_particles = 0;
    if (system_type == POINTS) {
        glBegin(GL_POINTS);
    }

    createNewParticles();
    for (int i = 0; i < max_particles; i++) {
        Particle *p = &particles[i];
        if (p->lifetime > 0.0f) {
            p->lifetime -= elapsed;
            if (p->lifetime > 0.0f) {
                p->speed = p->speed + (gravity * elapsed);
                p->pos = p->pos + (p->speed * elapsed / 10);
                p->cameradistance = length((p->pos - camera_position));
                if (system_type == POINTS){
                    glColor3f(p->color.r, p->color.g, p->color.b);
                    glVertex3dv(p->pos.unpack());
                }  else {
                    setGPUBuffers(p, active_particles);
                }
            } else {
                p->cameradistance = -1.0f; /* particle has just died */
            }
            active_particles++;
        }
    }
    if (system_type == POINTS) {
        glEnd();
    }
}