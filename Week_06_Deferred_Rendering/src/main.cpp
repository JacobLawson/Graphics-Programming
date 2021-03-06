#include "Week_06_Deferred_Rendering.h"

#define DEFAULT_SCREENWIDTH 1920
#define DEFAULT_SCREENHEIGHT 1080
// main that controls the creation/destruction of an application
int WinMain(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Week_06_Deferred_Rendering* app = new Week_06_Deferred_Rendering();
	app->run("Week_06_Deferred_Rendering_Tutorial", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}