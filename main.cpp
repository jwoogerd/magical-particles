/*
 * Jayme Woogerd and Louis Rassaby
 * Comp 175 - final project
 * April 18, 2014
 *
 * main.cpp 
 */

#include <string.h>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/glui.h>
#include <math.h>
#include "fountain.h"

/* These are the live variables passed into GLUI */
int main_window;
float view_rotate[16] = { 1,0,0,0, 
	                      0,1,0,0, 
	                      0,0,1,0, 
	                      0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, 0.0 };

GLUI *glui;

/* global variables */
enum SystemType {
	PARTICLE_FOUNTAIN,
	FOUNTAIN,
    REMCO_FOUNTAIN,
	FIRE_FOUNTAIN
};
/* global variables */
enum Human {
    REMCO,
    MIKE,
    PAUL,
    SOPHIE
};

std::string fountain_files[] = {"shaders/colorparticle.vert",
                                "shaders/colorparticle.frag", 
                                "shaders/particle.dds"};

std::string fire_files[] = {"shaders/fireparticle.vert",
                            "shaders/fireparticle.frag",
                            "shaders/firesprite.ppm"};


std::string remco_files[] = {"shaders/remcoparticle.vert",
                            "shaders/remcoparticle.frag",
                            "shaders/remcochang.ppm"};

SystemType current_number;
Human current_human_number;


Fountain pointfountain(POINTS);
Fountain fountain(DDS, 10, 10000, 1.0, fountain_files);
Fountain remcofountain(IMAGE, 10, 1000, 5.0, remco_files);
Fountain firefountain(IMAGE, 15, 1000, 2.0, fire_files);

ParticleSystem *current_system = &pointfountain;
int display_axes = false;
float dir_x, dir_y, dir_z;
float pos_x, pos_y, pos_z;
float spread, gravity, lifetime;
int num_particles, particle_size;

void getVariables() {
    dir_x = current_system->particle_direction[X];
    dir_y = current_system->particle_direction[Y];
    dir_z = current_system->particle_direction[Z];
    pos_x = current_system->position[X];
    pos_y = current_system->position[Y];
    pos_z = current_system->position[Z];
    spread = current_system->spread;
    num_particles = current_system->m_max_particles;
    particle_size = current_system->particle_size;
    gravity = current_system->gravity_y;
    lifetime = current_system->lifetime;
}

void pushVariables() {
    current_system->particle_direction[X] = dir_x;
    current_system->particle_direction[Y] = dir_y;
    current_system->particle_direction[Z] = dir_z;
    current_system->position[X] = pos_x;
    current_system->position[Y] = pos_y;
    current_system->position[Z] = pos_z;
    current_system->spread = spread;
    current_system->m_max_particles = num_particles;
    current_system->particle_size = particle_size;
    current_system->gravity_y = gravity;
    current_system->lifetime = lifetime;

}

void callbackSystemType (int id) {
    current_system->cleanup();
	switch(current_number) {
		case PARTICLE_FOUNTAIN:
			current_system = &pointfountain;
			break;
		case FOUNTAIN:
			current_system = &fountain;	
			break;
        case REMCO_FOUNTAIN:
            current_system = &remcofountain; 
            break;
		case FIRE_FOUNTAIN:
			current_system = &firefountain;
            break;	
		default:
			current_system = &pointfountain;	
			break;
	}
    current_system->initialize();
    getVariables();
    GLUI_Master.sync_live_all();
}

void callbackHuman (int id) {
    switch(current_human_number) {
        case REMCO:
            remcofountain.setHuman("shaders/remcochang.ppm", "shaders/remcoparticle.frag");
            break;
        case MIKE:
            remcofountain.setHuman("shaders/mikeshah.ppm", "shaders/remcoparticle.frag");
            break;
        case PAUL:
            remcofountain.setHuman("shaders/paulnixon.ppm", "shaders/remcoparticle.frag");
            break;
        case SOPHIE:
            remcofountain.setHuman("shaders/sophie.ppm", "shaders/remcoparticle.frag");
            break;  
        default:
            break;
    }
    if (current_system == &remcofountain) {
        current_system->initialize();
        getVariables();
        GLUI_Master.sync_live_all();
    } 
}

void myGlutIdle(void)
{
	/* According to the GLUT specification, the current window is
	undefined during an idle callback.  So we need to explicitly change
	it if necessary */
	if (glutGetWindow() != main_window){
		glutSetWindow(main_window);
	}

	glutPostRedisplay();
}

/*
	We configure our window so that it correctly displays our objects
	in the correct perspective.
*/
void myGlutReshape(int x, int y)
{
	float xy_aspect;
	xy_aspect = (float)x / (float)y;
 	glutFullScreen();
	//
	glViewport(0, 0, x, y);
	// Determine if we are modifying the camera(GL_PROJECITON) matrix(which is our viewing volume)
	glMatrixMode(GL_PROJECTION);
	// Reset the Projection matrix to an identity matrix
	glLoadIdentity();
	// The frustrum defines the perspective matrix and produces a perspective projection.
	// It works by multiplying the current matrix(in this case, our matrix is the GL_PROJECTION)
	// and multiplies it.
	glFrustum(-xy_aspect*.08, xy_aspect*.08, -.08, .08, .1, 15.0);
	// Since we are in projection mode, here we are setting the camera to the origin (0,0,0)
	glTranslatef(0, 0, 0);
	// Call our display function.
	glutPostRedisplay();
}

void drawAxis(){                                                                    
	glUseProgram(0);
    glBegin(GL_LINES);                                                              
        glColor3f(1.0, 0.0, 0.0);                                                   
        glVertex3f(0, 0, 0); glVertex3f(1.0, 0, 0);                                 
        glColor3f(0.0, 1.0, 0.0);                                                   
        glVertex3f(0, 0, 0); glVertex3f(0.0, 1.0, 0);                               
        glColor3f(0.0, 0.0, 1.0);                                                   
        glVertex3f(0, 0, 0); glVertex3f(0, 0, 1.0);                                 
    glEnd();                                                                        
}    

void myGlutDisplay(void)
{
	// Clear the buffer of colors in each bit plane.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//allow for user controlled rotation and scaling
	glTranslatef(obj_pos[0], obj_pos[1]-0.25, -obj_pos[2]-1.2);
	glMultMatrixf(view_rotate);

	// draw the fountain
    pushVariables(); 
	current_system->drawParticles();
	if (display_axes) {
		drawAxis();
	}
	glutSwapBuffers();
}

void reset() {
    pointfountain = Fountain(POINTS);
    fountain = Fountain(DDS, 10, 10000, 1.0, fountain_files);
    remcofountain = Fountain(IMAGE, 10, 1000, 5.0, remco_files);
    firefountain = Fountain(IMAGE, 15, 1000, 2.0, fire_files);
    callbackHuman(-1);

    memset(view_rotate, 0, sizeof(float) * 16);
    view_rotate[0] = 1;
    view_rotate[5] = 1;
    view_rotate[10] = 1;
    view_rotate[15] = 1;
    memset(obj_pos, 0, sizeof(float) * 3);

    callbackSystemType(-1);
}

int main(int argc, char* argv[])
{
	/****************************************/
	/*   Initialize GLUT and create window  */
	/****************************************/

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);


	main_window = glutCreateWindow("Magical Particle Systems");
	glutDisplayFunc(myGlutDisplay);
	glutReshapeFunc(myGlutReshape);


	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(-1);
	}

	/* Ensure that instancing is supported by GPU */	
    if (!GL_ARB_draw_instanced || !GL_ARB_instanced_arrays) {
        fprintf(stderr, "This GPU does not support instancing.");
        exit(-1);
    }

	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	glui = GLUI_Master.create_glui_subwindow( main_window, GLUI_SUBWINDOW_BOTTOM );

		
    GLUI_Panel *particle_systems= glui->add_panel("Particle Systems");
	GLUI_RadioGroup *selected = glui->add_radiogroup_to_panel(particle_systems, (int*)(&current_number), 3, callbackSystemType);
	glui->add_radiobutton_to_group(selected, "Particle Fountain");
	glui->add_radiobutton_to_group(selected, "Fountain");
    glui->add_radiobutton_to_group(selected, "Remco Fountain");
	glui->add_radiobutton_to_group(selected, "Fire Fountain");
    new GLUI_Column( glui, false );

	
	GLUI_Panel *object_panel = glui->add_panel("Objects");
	GLUI_Rotation *view_rot = new GLUI_Rotation(object_panel, "Objects", view_rotate );
    view_rot->set_spin( 1.0 );
    // Navigate our scene
    new GLUI_Column( object_panel, false );
    GLUI_Translation *trans_x =  new GLUI_Translation(object_panel, "Objects X", GLUI_TRANSLATION_X, &obj_pos[0]);
    trans_x->set_speed( .1 );
    new GLUI_Column( object_panel, false );
    GLUI_Translation *trans_y =  new GLUI_Translation(object_panel, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1]);
    trans_y->set_speed( .1 );
    new GLUI_Column( object_panel, false );
    GLUI_Translation *trans_z =  new GLUI_Translation(object_panel, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2]);
    trans_z->set_speed( .1 );
    new GLUI_Column( glui, false );
    GLUI_Panel *particles_panel = glui->add_panel("Particles");

    (new GLUI_Spinner(particles_panel, "Num particles", &num_particles))->set_int_limits(10, 50000);
    (new GLUI_Spinner(particles_panel, "Spread", &spread))->set_float_limits(0.1, 10.0);
    (new GLUI_Spinner(particles_panel, "Particle Size", &particle_size))->set_int_limits(1, 100);
    new GLUI_Column( particles_panel, false );
    (new GLUI_Spinner(particles_panel, "Gravity", &gravity))->set_float_limits(-50.0, 50.0);
    (new GLUI_Spinner(particles_panel, "Lifetime", &lifetime))->set_float_limits(0.1, 15.0);
    new GLUI_Column( glui, false );


    GLUI_Panel *direction_panel = glui->add_panel("Main Direction");
    (new GLUI_Spinner(direction_panel, "X", &dir_x))->set_float_limits(-50.0, 50.0);
    (new GLUI_Spinner(direction_panel, "Y", &dir_y))->set_float_limits(-50.0, 50.0);
    (new GLUI_Spinner(direction_panel, "Z", &dir_z))->set_float_limits(-50.0, 50.0);
    new GLUI_Column( glui, false );

    GLUI_Panel *position_panel = glui->add_panel("Position");
    (new GLUI_Spinner(position_panel, "X", &pos_x))->set_float_limits(-5.0, 5.0);
    (new GLUI_Spinner(position_panel, "Y", &pos_y))->set_float_limits(-5.0, 5.0);
    (new GLUI_Spinner(position_panel, "Z", &pos_z))->set_float_limits(-5.0, 5.0);
    new GLUI_Column( glui, false );

    GLUI_Panel *staff_panel = glui->add_panel("Graphics Peeps");
    GLUI_RadioGroup *person = glui->add_radiogroup_to_panel(staff_panel, (int*)(&current_human_number), 3, callbackHuman);
    glui->add_radiobutton_to_group(person, "Remco");
    glui->add_radiobutton_to_group(person, "Mike");
    glui->add_radiobutton_to_group(person, "Paul");
    glui->add_radiobutton_to_group(person, "Sophie");

    new GLUI_Column( glui, false );
     
	glui->add_checkbox("Display Axes", &display_axes);
	glui->add_button("Reset", 0, (GLUI_Update_CB)reset);
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	current_system->initialize();
    getVariables();
    GLUI_Master.sync_live_all();

	glutMainLoop();

	return EXIT_SUCCESS;
}
