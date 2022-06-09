#include "Week_03_Lighting_Model_Loading.h"

#define DEFAULT_SCREENWIDTH 1920
#define DEFAULT_SCREENHEIGHT 1080
// main that controls the creation/destruction of an application
int WinMain(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Week_03_Lighting_Model_Loading* app = new Week_03_Lighting_Model_Loading();
	app->run("Week_03_Lighting_Model_Loading Project", DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT, false);

	// explicitly control the destruction of our application
	delete app;

	return 0;
}