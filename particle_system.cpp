#include "particle_system.h"
#include <stdio.h>
#include <algorithm>
#include "ppm.h"

#include "common/shader.h"
#include "common/texture.hpp"

ParticleSystem::ParticleSystem()
{
    initialized = false;
    m_max_particles = 10000;
    last_used_particle = 0;
    lifetime = 5.0;
    spread = 1.0f; 
    radius = 1.5f;
    position = Point(0.0, 0.0, 0.0);
    srand (time(NULL));
}

ParticleSystem::~ParticleSystem() 
{
    cleanup();
    particles.clear();
    if (initialized && system_type != POINTS) {
        delete [] position_size_data;
        delete [] color_data;
        delete [] age_data;
    }
}

void ParticleSystem::initialize()
{
    initialized = true;
    Shader manager;

    /* sync data */
    gravity_y = gravity[Y];
    max_particles = m_max_particles;
    particle_direction[X] = main_direction[X];
    particle_direction[Y] = main_direction[Y];
    particle_direction[Z] = main_direction[Z];

    particles.resize(max_particles);
    getCameraMatrices();

    if (system_type != POINTS) {
        position_size_data = new GLfloat[max_particles * 4];
        color_data = new GLubyte[max_particles * 4];
        age_data = new GLfloat[max_particles];

        // Create and compile our GLSL program from the shaders
        programID = manager.loadShader(vertex_shader.c_str(), fragment_shader.c_str());

        // Vertex shader
        CameraRight_worldspace_ID = glGetUniformLocation(programID, "CameraRight_worldspace");
        CameraUp_worldspace_ID = glGetUniformLocation(programID, "CameraUp_worldspace");
        ViewProjMatrixID = glGetUniformLocation(programID, "VP");

        // fragment shader
        texture_ID  = glGetUniformLocation(programID, "myTextureSampler");

        /* get handle for buffers */
        squareVerticesID = glGetAttribLocation(programID, "squareVertices");
        xyzsID = glGetAttribLocation(programID, "xyzs");
        colorID = glGetAttribLocation(programID, "color");
        ageID = glGetAttribLocation(programID, "age");

        /* load dds files */
        if (system_type == DDS){
            texture = loadDDS(texture_file.c_str());
        }
        /* load ppm files */
        else if (system_type == IMAGE) {
            ppm image(texture_file);
            texture = image.createAsTexture();
        }

        /* VBO containing the particle billboard used to instance particles */
        glGenBuffers(1, &billboard_vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), 
                     g_vertex_buffer_data, GL_STATIC_DRAW);

        /* VBO containing the positions and sizes of the particles */
        glGenBuffers(1, &particles_position_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLfloat), NULL, 
                     GL_STREAM_DRAW);

        /* The VBO containing particle colors */
        glGenBuffers(1, &particles_color_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLubyte), NULL, 
                     GL_STREAM_DRAW);

        /* The VBO containing particle age */
        glGenBuffers(1, &particles_age_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, particles_age_buffer);
        glBufferData(GL_ARRAY_BUFFER, max_particles * sizeof(GLfloat), NULL, 
                     GL_STREAM_DRAW);
    }
    last_time = glutGet(GLUT_ELAPSED_TIME);
}

void ParticleSystem::setGPUBuffers(Particle *particle, int particle_index)
{
    int i = 4 * particle_index;
    position_size_data[i + X] = particle->pos[X];
    position_size_data[i + Y] = particle->pos[Y];
    position_size_data[i + Z] = particle->pos[Z];
    position_size_data[i + SIZE] = particle->size;

    color_data[i + R] = particle->color.r * 255;
    color_data[i + G] = particle->color.g * 255;
    color_data[i + B] = particle->color.b * 255;
    color_data[i + A] = particle->color.a * 255;

    if (system_type == IMAGE) {
        GLfloat age = 1.0 - (particle->lifetime / lifetime);
        if (age > 1.0) age = 1.0;
        age_data[particle_index] = age;
    }
}

void ParticleSystem::drawParticles() 
{
    gravity = Vector(0.0, gravity_y, 0.0);
    main_direction = Vector(particle_direction[X], 
                            particle_direction[Y], 
                            particle_direction[Z]);
    int current_time = glutGet(GLUT_ELAPSED_TIME);
    elapsed = (current_time - last_time) * 0.001f;
    last_time = current_time;

    getCameraMatrices();

    if (max_particles != m_max_particles) {
        max_particles = m_max_particles;
        particles.resize(max_particles);

        if (system_type != POINTS) {
            delete [] position_size_data;
            delete [] color_data;
            delete [] age_data;
            position_size_data = new GLfloat[max_particles * 4];
            color_data = new GLubyte[max_particles * 4];
            age_data = new GLfloat[max_particles];
        }
    }

    computeParticles();

    if (system_type != POINTS) {
        sortParticles();
        bindShaders();
    }
}

/*
 * findUnusedParticle - returns the index of the first dead particle 
 * (i.e. life < 0) in the particles vector.
 */
 int ParticleSystem::findDeadParticle()
 {
    for(int i = last_used_particle; i < max_particles; i++) {
        if (particles[i].lifetime < 0) {
            last_used_particle = i;
            return i;
        }
    }
    for(int i = 0; i < last_used_particle; i++) {
    	if (particles[i].lifetime <= 0) {
                last_used_particle = i;
            return i;
        }
    }
    return 0; /* all particles are taken, override the first one */
}

/* 
 * bindShaders - bind buffer data and pass shader variables.
 * code based on example in http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/ 
 */
void ParticleSystem::bindShaders()
{
    /* Update the buffers that OpenGL uses for rendering */
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, active_particles * sizeof(GLfloat) * 4, position_size_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, active_particles * sizeof(GLubyte) * 4, color_data);

    glBindBuffer(GL_ARRAY_BUFFER, particles_age_buffer);
    glBufferData(GL_ARRAY_BUFFER, max_particles * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
    glBufferSubData(GL_ARRAY_BUFFER, 0, active_particles * sizeof(GLfloat), age_data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use our shader
    glUseProgram(programID);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(texture_ID, 0);

    glUniform3f(CameraRight_worldspace_ID, model_view[0][0], model_view[1][0], model_view[2][0]);
    glUniform3f(CameraUp_worldspace_ID, model_view[0][1], model_view[1][1], model_view[2][1]);
    glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &model_projection[0][0]);

    /* billboard vertices */
    glEnableVertexAttribArray(squareVerticesID);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(squareVerticesID, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    /* bind particle attributes */

    /* positions of particles' centers */
    glEnableVertexAttribArray(xyzsID);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(xyzsID, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);

    /* particles' colors */
    glEnableVertexAttribArray(colorID);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(colorID, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void *)0);

    /* particles' age */
    glEnableVertexAttribArray(ageID);
    glBindBuffer(GL_ARRAY_BUFFER, particles_age_buffer);
    glVertexAttribPointer(ageID, 1, GL_FLOAT, GL_TRUE, 0, (void *)0);

    glVertexAttribDivisorARB(squareVerticesID, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisorARB(xyzsID, 1); // positions: one per quad (its center)                 -> 1
    glVertexAttribDivisorARB(colorID, 1); // color: one per quad                                  -> 1
    glVertexAttribDivisorARB(ageID, 1); /* age: one per quad */
                             
    /* draw each instance */
    glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, active_particles);

    glDisableVertexAttribArray(squareVerticesID);
    glDisableVertexAttribArray(xyzsID);
    glDisableVertexAttribArray(colorID);
    glDisableVertexAttribArray(ageID);
}

/* getRandVector - returns a vector in a random direction */
Vector ParticleSystem::getRandVector()
{
    float phi = (rand() / (float)RAND_MAX) * 2 * M_PI;
    float costheta = (2 * (rand() / (float)RAND_MAX)) - 1;
    float u = (rand() / (float)RAND_MAX);

    float theta = acos(costheta);
    float r = radius * pow(u, (1/3));

    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);

    return Vector(x, y, z);
}

/*
 * getCameraMatrices - sets the model_view and model_projection matrices and 
 * camera_position Point.
 */
void ParticleSystem::getCameraMatrices()
{
    GLfloat mv[16];
    GLfloat p[16];
    glm::mat4 projection;
    glm::mat4 inverse_model_view;

    glGetFloatv(GL_MODELVIEW_MATRIX, mv);
    glGetFloatv(GL_PROJECTION_MATRIX, p);

    model_view = glm::make_mat4(mv);
    projection = glm::make_mat4(p);
    model_projection = projection * model_view;
    inverse_model_view = glm::inverse(model_view);

    camera_position = Point(inverse_model_view[3][0], 
                            inverse_model_view[3][1], 
                            inverse_model_view[3][2]);

}

void ParticleSystem::cleanup() 
{
    glDeleteBuffers(1, &particles_position_buffer);
    glDeleteBuffers(1, &billboard_vertex_buffer);
    glDeleteBuffers(1, &particles_color_buffer);
    glDeleteBuffers(1, &particles_age_buffer);
    glDeleteTextures(1, &texture_ID);
    glDeleteProgram(programID);
    glUseProgram(0);
}

void ParticleSystem::sortParticles()
{
    std::sort(&particles[0], &particles[max_particles]);
}