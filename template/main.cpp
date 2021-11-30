
#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app_camera.h"



class App_viewer: public AppCamera
{
public:
    App_viewer(): AppCamera(1024, 640) {}
    
    int init()
	{
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

		glClearDepth(1.f);                          // profondeur par defaut
		glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
		glEnable(GL_DEPTH_TEST);
        
        return 0;
    }
    
    int quit()
	{
        return 0;
    }
    
    
    int update(const float time, const float delta) {
		return 0;
	}

    
    int render()
	{
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return 1;
    }
    
protected:
    Orbiter m_camera;
};


int main(int argc, char** argv) {
	App_viewer runner;
	runner.run();
	runner.quit();
	return 0;
}