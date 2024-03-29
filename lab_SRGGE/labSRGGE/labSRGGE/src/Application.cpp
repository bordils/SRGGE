#include <GL/glew.h>
#include <GL/glut.h>
#include "Application.h"
#include <iostream>
#include "Camera.h"

// camera
//float lastX = (float)SCR_WIDTH / 2.0;
//float lastY = (float)SCR_HEIGHT / 2.0;
float lastX = (float)790 / 2.0;
float lastY = (float)860 / 2.0;
bool firstMouse = true;

void Application::init()
{
	bPlay = true;
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	scene.init();
	
	
	for(unsigned int i=0; i<256; i++)
	{
	  keys[i] = false;
	  specialKeys[i] = false;
	}
	mouseButtons[0] = false;
	mouseButtons[1] = false;
	lastMousePos = glm::ivec2(-1, -1);
}

bool Application::loadMesh(const char *filename)
{
  return scene.loadMesh(filename);
}

bool Application::update(int deltaTime)
{
	scene.update(deltaTime);
	
	return bPlay;
}

void Application::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.render();
}

void Application::resize(int width, int height)
{
  glViewport(0, 0, width, height);
  scene.getCamera().resizeCameraViewport(width, height);
}

void Application::keyPressed(int key)
{
	//bool  forward, back, left, right;
	if (key == 27) // Escape code
	{
		bPlay = false;
	}
	else if (key == 119) // w
		scene.getCamera().processKeyboard(0);
	else if (key == 115) // s
		scene.getCamera().processKeyboard(1);
	else if (key == 97) // a
		scene.getCamera().processKeyboard(2);
	else if (key == 100) // d
		scene.getCamera().processKeyboard(3);
	keys[key] = true;
}

void Application::keyReleased(int key)
{
	keys[key] = false;
}

void Application::specialKeyPressed(int key)
{
	specialKeys[key] = true;
}

void Application::specialKeyReleased(int key)
{
	specialKeys[key] = false;
	if(key == GLUT_KEY_F1)
	  scene.switchPolygonMode();
}

void Application::mouseMove(int x, int y)
{

	// Rotation
	if(mouseButtons[0] && lastMousePos.x != -1)
	  scene.getCamera().rotateCamera((y - lastMousePos.y), (x - lastMousePos.x));

	// Zoom
	if(mouseButtons[1] && lastMousePos.x != -1)
	  scene.getCamera().zoomCamera(0.01f * (y - lastMousePos.y), GLUT_SCREEN_WIDTH, GLUT_SCREEN_HEIGHT );

 	lastMousePos = glm::ivec2(x, y);
}

void Application::mousePress(int button)
{
  mouseButtons[button] = true;
}

void Application::mouseRelease(int button)
{
  mouseButtons[button] = false;
  if(!mouseButtons[0] && !mouseButtons[1])
    lastMousePos = glm::ivec2(-1, -1);
}

bool Application::getKey(int key) const
{
	return keys[key];
}

bool Application::getSpecialKey(int key) const
{
	return specialKeys[key];
}





