#pragma once

#include "core/InputHandler.h"
#include "core/FPSCamera.h"
#include "core/WindowManager.hpp"
#include <random>
#include "core/node.hpp"

class Window;


namespace edaf80
{
	class ParticleSpawner3D
	{
	public:
		ParticleSpawner3D();
		~ParticleSpawner3D();

		int numParticlesPerAxis = 2;//20
		int particleCount = 8;//8000
		glm::vec3 centre = glm::vec3(0,-0.47,0);
		float size = 3.7f;
		glm::vec3 initialVel;
		float jitterStrength;

		struct ParticleSpawnData {

			std::vector<glm::vec3> positions;
			std::vector<glm::vec3> velocities;

			ParticleSpawnData(int num) {
				positions.resize(num);
				velocities.resize(num);
			}
		};
		ParticleSpawnData GetSpawnData() {
			int numPoints = numParticlesPerAxis * numParticlesPerAxis * numParticlesPerAxis;
			ParticleSpawnData data(numPoints);
			int i = 0;
			for (int x = 0; x < numParticlesPerAxis; x++) {
				for (int y = 0; y < numParticlesPerAxis; y++) {
					for (int z = 0; z < numParticlesPerAxis; z++) {
						float tx = x / (numParticlesPerAxis - 1.0f);
						float ty = y / (numParticlesPerAxis - 1.0f);
						float tz = z / (numParticlesPerAxis - 1.0f);
						float px = (tx - 0.5f) * size + centre.x;
						float py = (ty - 0.5f) * size + centre.y;
						float pz = (tz - 0.5f) * size + centre.z;
						glm::vec3 jitter;
						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
						jitter.x = dis(gen) * jitterStrength;
						jitter.y = dis(gen) * jitterStrength;
						jitter.z = dis(gen) * jitterStrength;
						data.positions[i] = { px, py, pz };
						data.velocities[i] = initialVel;
						i++;
					}
				}
			}
			return data;
		}

	private:

	};
	//! \brief Wrapper class for Assignment 5
	class project3D {
	public:
		//! \brief Default constructor.
		//!
		//! It will initialise various modules of bonobo and retrieve a
		//! window to draw to.
		project3D(WindowManager& windowManager);

		//! \brief Default destructor.
		//!
		//! It will release the bonobo modules initialised by the
		//! constructor, as well as the window.
		~project3D();

		//! \brief Contains the logic of the assignment, along with the
		//! render loop.
		void run();
		void boundaryCollisions(glm::vec3* position, glm::vec3* velocity);
		GLuint compileShader(GLenum shaderType, const std::string& source);
		GLuint createComputeShaderProgram(const std::string& computeShaderSource);
		std::string readFile(const std::string& filePath);
		ParticleSpawner3D spawner = ParticleSpawner3D();
		struct particleParameter {
			glm::vec3 position;
			glm::vec3 velocity;
			glm::vec3 predictedPosition;
			glm::vec2 density;
			glm::uvec3 spatialIndices;
			unsigned int spatial;
		};
	private:
		FPSCameraf     mCamera;
		InputHandler   inputHandler;
		WindowManager& mWindowManager;
		GLFWwindow* window;

		//project parameter
		unsigned int particlesNum = 8; //8000
		float particleRadius = 0.05f;//0.02f;
		float PoolHeight = 9.0f;
		float poolWidth = 17.1f;
		float collisionDamping = 0.8f;
		float gravity = -10.0f;
		glm::vec2 boundsSize = glm::vec2(17.1f, 9.0f);

		float smoothingRadius = 0.2f;
		float targetDensity = 630.0f;
		float pressureMultiplier = 288.0f;
		float nearPressureMultiplier = 2.25f;
		float viscosityStrength = 0.001f;

		float pi = 3.14159265359f;

	};

}
