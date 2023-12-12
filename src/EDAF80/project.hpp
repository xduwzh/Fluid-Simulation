#pragma once

#include "core/InputHandler.h"
#include "core/FPSCamera.h"
#include "core/WindowManager.hpp"
#include <random>
#include "core/node.hpp"

class Window;


namespace edaf80
{
	class ParticleSpawner
	{
	public:
		ParticleSpawner();
		~ParticleSpawner();
		unsigned int particleCount = 10000;//4096

		glm::vec2 initialVelocity = glm::vec2(0.0f, 0.0f);
		glm::vec2 spawnCentre = glm::vec2(2.0f, 0.5f);
		glm::vec2 spawnSize = glm::vec2(7.0f, 5.0f);//7,5
		float jitterStr = 0.025;
		bool showSpawnBoundsGizmos;

		struct ParticleSpawnData {
			std::vector<glm::vec2> positions;
			std::vector<glm::vec2> velocities;

			ParticleSpawnData(int num) {
				positions.resize(num);
				velocities.resize(num);
			}
		};
		ParticleSpawnData GetSpawnData() {
			ParticleSpawnData data(particleCount);
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<float> dis(0.0f, 1.0f);
			glm::vec2 s = spawnSize;
			int numX = std::ceil(std::sqrt(s.x / s.y * particleCount + (s.x - s.y) * (s.x - s.y) / (4 * s.y * s.y)) - (s.x - s.y) / (2 * s.y));
			int numY = std::ceil(particleCount / static_cast<float>(numX));
			int i = 0;
			for (int y = 0; y < numY; y++) {
				for (int x = 0; x < numX; x++) {
					if (i >= particleCount) break;
					float tx = numX <= 1 ? 0.5f : x / (numX - 1.0f);
					float ty = numY <= 1 ? 0.5f : y / (numY - 1.0f);
					float angle = dis(gen) * 3.14f * 2;
					glm::vec2 dir(std::cos(angle), std::sin(angle));
					glm::vec2 jitter = dir * jitterStr * (dis(gen) - 0.5f);
					data.positions[i] = glm::vec2((tx - 0.5f) * spawnSize.x, (ty - 0.5f) * spawnSize.y) + jitter + spawnCentre;
					data.velocities[i] = glm::vec2(glm::vec2((tx - 0.5f) * spawnSize.x, (ty - 0.5f) * spawnSize.y) + jitter + spawnCentre) * 2.0f;
					i++;
				}
			}
			return data;
		}

	private:

	};
	//! \brief Wrapper class for Assignment 5
	class project {
	public:
		//! \brief Default constructor.
		//!
		//! It will initialise various modules of bonobo and retrieve a
		//! window to draw to.
		project(WindowManager& windowManager);

		//! \brief Default destructor.
		//!
		//! It will release the bonobo modules initialised by the
		//! constructor, as well as the window.
		~project();

		//! \brief Contains the logic of the assignment, along with the
		//! render loop.
		void run();
		void boundaryCollisions(glm::vec3* position, glm::vec3* velocity);
		GLuint compileShader(GLenum shaderType, const std::string& source);
		GLuint createComputeShaderProgram(const std::string& computeShaderSource);
		std::string readFile(const std::string& filePath);
		ParticleSpawner spawner = ParticleSpawner();
		struct particleParameter {
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec2 predictedPosition;
			glm::vec2 density;
			glm::uvec3 spatialIndices;
			unsigned int spatial;
		};
		glm::vec2 CalculateDensity1(std::vector<particleParameter> particles);
	private:
		FPSCameraf     mCamera;
		InputHandler   inputHandler;
		WindowManager& mWindowManager;
		GLFWwindow* window;

		//project parameter
		unsigned int particlesNum = 10000; //4096
		float particleRadius = 0.02f;//0.02f;
		float PoolHeight = 9.0f;
		float poolWidth = 17.1f;
		float collisionDamping = 0.8f;
		float gravity = -6.0f;
		glm::vec2 boundsSize = glm::vec2(17.1f, 9.0f);

		float smoothingRadius = 0.35f;
		float targetDensity = 55.0f;//55
		float pressureMultiplier = 500.0f;
		float nearPressureMultiplier = 18.0f;
		float viscosityStrength = 0.06f;

		float pi = 3.14159265359f;
		
	};

}
