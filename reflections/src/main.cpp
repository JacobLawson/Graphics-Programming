#include "Reflection.h"

#define DEFAULT_SCREENWIDTH 1024
#define DEFAULT_SCREENHEIGHT 720
// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Reflection* app = new Reflection();
	app->run("Reflection", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}