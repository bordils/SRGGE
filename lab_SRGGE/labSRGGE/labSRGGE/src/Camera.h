#ifndef _CAMERA_INCLUDE
#define _CAMERA_INCLUDE

#include <glm/glm.hpp>

// Default camera values
const float YAW = 180.0f;
const float PITCH = 0.0f;
const float SPEED = 0.1f;
const float SENSITIVITY = 0.1f; // 0.1f
const float ZOOM = 45.0f;

// Camera contains the properies of the camera the scene is using
// It is responsible for computing the associated GL matrices

class Camera
{
public:
	// Camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// Euler Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	Camera(glm::vec3 position = glm::vec3(-1.1f, 0.5f, -1.1f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	void resizeCameraViewport(int width, int height);
	void rotateCamera(float xRotation, float yRotation);
	void zoomCamera(float yoffset, int SCR_WIDTH, int SCR_HEIGHT);
	void processKeyboard(int direction);

	glm::mat4 &getProjectionMatrix();
	glm::mat4 &getViewMatrix();
	glm::vec3 &getCameraPosition();

private:
	void computeViewMatrix();
	void updateCameraVectors();

	glm::mat4 projection, view;  // OpenGL matrices
};


#endif // _CAMERA_INCLUDE

