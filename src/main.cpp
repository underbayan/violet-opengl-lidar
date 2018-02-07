#include "./RenderAPI.h"
int main () {
	RENDER_API::initGlfw();
	RENDER_API::generatePoints("./ressources/20170817-145631.dat",1,5);
	RENDER_API::loadImg("./ressources/01.png");
	RENDER_API::updateGLBuffer();
	RENDER_API::destoryGlfw();
	return 0;
}


