CC= clang++

LCFLAGS_QUIET= -w -g -O2
LCFLAGS_VERBOSE= -g -Wall -Wextra
LLFLAGS= -I/usr/local/Cellar/glew/1.10.0/include -L/usr/local/Cellar/glew/1.10.0/lib -L/usr/local/Cellar/glui/2.36/lib -I/usr/local/Cellar/glui/2.36/include -lglew -framework OpenGL -framework GLUT -lglui -lpthread

MCFLAGS = -Wall -Wextra -stdlib=libstdc++
MCLIBS = /Library/Frameworks/GLUI.framework/Headers
MCFRAME = -framework OpenGL -framework GLUT -framework GLUI

TARGET = run

CFILES = main.cpp particle.cpp particle_system.cpp fountain.cpp remcofountain.cpp firefountain.cpp common/shader.cpp common/texture.cpp ppm.cpp 

all: louis

louis: $(CFILES)
	$(CC) $(LCFLAGS_QUIET) $(CFILES) $(LLFLAGS) -o $(TARGET)

v: $(CFILES) 
	$(CC) $(LCFLAGS_VERBOSE) $(CFILES) $(LLFLAGS) -o $(TARGET)

jayme: $(CFILES)
	$(CC) $(MCFLAGS) $(CFILES) -I $(MCLIBS) $(MCFRAME) -o $(TARGET)

clean:
	rm $(TARGET)
