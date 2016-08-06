#ifndef PRESENTATION_H
#define PRESENTATION_H

#include <GL/glew.h>

// The presentationStage represents different stages for demonstration
// purposes. 0 is the most primative, each step beyond that increments by
// one. Initialized to a high value to show the full glory on first load.
GLuint getPresentationStage(void);
void setPresentationStage(GLuint);

#endif /* PRESENTATION_H */
