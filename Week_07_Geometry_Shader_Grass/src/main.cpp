#include "Week_07_Geometry_Shader_Grass.h"

#define DEFAULT_SCREENWIDTH 1920
#define DEFAULT_SCREENHEIGHT 1080
// main that controls the creation/destruction of an application
int WinMain(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Week_07_Geometry_Shader_Grass* app = new Week_07_Geometry_Shader_Grass();
	app->run("Week_07_Geometry_Shader", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}