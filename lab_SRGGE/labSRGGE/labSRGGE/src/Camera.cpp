#include "Camera.h"
#include "Camera.h"
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


#define PI 3.14159f


void Camera::resizeCameraViewport(int width, int height)
{
	projection = glm::perspective(60.f / 180.f * PI, float(width) / float(height), 0.01f, 100.0f);
}

void Camera::rotateCamera(float yoffset, float xoffset)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//Yaw += xoffset;
	//Pitch -= yoffset;

	Yaw += xoffset;
	Pitch += yoffset;

	updateCameraVectors();
}

void Camera::zoomCamera(float yoffset, int SCR_WIDTH, int SCR_HEIGHT)
{
	Zoom += yoffset;
	if (Zoom < 1)
		Zoom = 1;
	if (Zoom > 100)
		Zoom = 100; 
	projection = glm::perspective(glm::radians(Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	updateCameraVectors();
}

void Camera::computeViewMatrix()
{
	view = glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 &Camera::getProjectionMatrix()
{
	return projection;
}

glm::mat4 &Camera::getViewMatrix()
{
	computeViewMatrix();
	return view;
}

glm::vec3 & Camera::getCameraPosition()
{
	return Position;
}

void Camera::processKeyboard(int direction)
{

	if (direction == 0) 
		Position += Front * MovementSpeed;
	if (direction == 1)
		Position -= Front * MovementSpeed;
	if (direction == 2)
		Position -= Right * MovementSpeed;
	if (direction == 3)
		Position += Right * MovementSpeed;

	Position.y = 0.5;
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 front;

	front.x = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = -sin(glm::radians(Pitch));
	front.z = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}



