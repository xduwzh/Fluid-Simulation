#include "assignment5.hpp"
#include "parametric_shapes.hpp"
#include "interpolation.hpp"

#include "config.hpp"
#include "core/Bonobo.h"
#include "core/FPSCamera.h"
#include "core/helpers.hpp"
#include "core/node.hpp"
#include "core/ShaderProgramManager.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <tinyfiledialogs.h>

#include <clocale>
#include <stdexcept>

#include <vector>
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


edaf80::Assignment5::Assignment5(WindowManager& windowManager) :
	mCamera(0.5f * glm::half_pi<float>(),
	        static_cast<float>(config::resolution_x) / static_cast<float>(config::resolution_y),
	        0.01f, 1000.0f),
	inputHandler(), mWindowManager(windowManager), window(nullptr)
{
	WindowManager::WindowDatum window_datum{ inputHandler, mCamera, config::resolution_x, config::resolution_y, 0, 0, 0, 0};

	window = mWindowManager.CreateGLFWWindow("EDAF80: Assignment 5", window_datum, config::msaa_rate);
	if (window == nullptr) {
		throw std::runtime_error("Failed to get a window: aborting!");
	}

	bonobo::init();
}

edaf80::Assignment5::~Assignment5()
{
	bonobo::deinit();
}
float judgeDirection(float cos_theta, float sin_theta) {
	 float acos_theta = acos(cos_theta);
	 float asin_theta = asin(sin_theta);
	if (cos_theta > 0 && sin_theta > 0) {
		return asin_theta;
	}
	else if (cos_theta < 0 && sin_theta > 0) {
		return glm::pi<float>() - asin_theta;
	}
	else if (cos_theta < 0 && sin_theta < 0) {
		return glm::pi<float>() - asin_theta;
	}
	else if (cos_theta > 0 && sin_theta < 0) {
		return glm::two_pi<float>() + asin_theta;
	}
	else if (cos_theta == 1 && sin_theta == 0) {
		return 0;
	}
	else if (cos_theta == -1 && sin_theta == 0) {
		return glm::pi<float>();
	}
	else if (cos_theta == 0 && sin_theta == 1) {
		return glm::pi<float>() / 2;
	}
	else if (cos_theta == 0 && sin_theta == -1) {
		return 3 * glm::pi<float>() / 2;
	}
}
float rotateTurret(Node turret, glm::vec3 targetDirection) {
	float curRotationyCos = turret.get_transform().GetRotation()[0][0];
	float curRotationySin = turret.get_transform().GetRotation()[2][0];
	float curRoationAngle = judgeDirection(curRotationyCos, curRotationySin);
	float targetDirectionAngle = judgeDirection(targetDirection.z, targetDirection.x);
	bool isboatrotate;
	if (abs(curRoationAngle - targetDirectionAngle) < 0.04) {
		isboatrotate = false;
		return curRoationAngle;
	}
	else {
		isboatrotate = true;;
	}
	if (curRoationAngle != targetDirectionAngle && isboatrotate == true) {
		if (curRoationAngle - targetDirectionAngle > 0) {
			if (curRoationAngle - targetDirectionAngle > glm::pi<float>()) {
				return curRoationAngle + 0.04f;
			}
			else {
				return curRoationAngle - 0.04f;
			}
		}
		else {
			if (targetDirectionAngle - curRoationAngle > glm::pi<float>()) {
				return curRoationAngle - 0.04f;
			}
			else {
				return curRoationAngle + 0.04f;
			}
		}
	}
}
float rotateTurret(Node turret, glm::vec3 targetDirection, float parentRotationAngle) {
	float curRotationyCos = turret.get_transform().GetRotation()[0][0];
	float curRotationySin = turret.get_transform().GetRotation()[2][0];
	float curRoationAngle_local = judgeDirection(curRotationyCos, curRotationySin);
	float curRoationAngle_word = curRoationAngle_local + parentRotationAngle;
	if (curRoationAngle_word > glm::two_pi<float>()) {
		curRoationAngle_word -= glm::two_pi<float>();
	}
	float targetDirectionAngle = judgeDirection(targetDirection.z, targetDirection.x);
	bool isboatrotate;
	if (abs(curRoationAngle_word - targetDirectionAngle) < 0.06) {
		isboatrotate = false;
		return curRoationAngle_local;
	}
	else {
		isboatrotate = true;;
	}
	if (curRoationAngle_word != targetDirectionAngle && isboatrotate == true) {
		if (curRoationAngle_word - targetDirectionAngle > 0) {
			if (curRoationAngle_word - targetDirectionAngle > glm::pi<float>()) {
				return curRoationAngle_local + 0.06f;
			}
			else {
				return curRoationAngle_local - 0.06f;
			}
		}
		else {
			if (targetDirectionAngle - curRoationAngle_word > glm::pi<float>()) {
				return curRoationAngle_local - 0.06f;
			}
			else {
				return curRoationAngle_local + 0.06f;
			}
		}
	}
}
void timerPrint(const char* word,float printNunber,  float duration, float& curTimer, float durationTime) {
	curTimer += durationTime;
	if (curTimer > duration) {
		std::cout << word << printNunber << std::endl;
		curTimer = 0;
	}
}
glm::vec3 getRandomPointInRadius(const glm::vec3& center, float minRadius, float maxRadius) {
	// 初始化随机数生成器
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

	// 生成随机方向
	float theta = glm::two_pi<float>() * distribution(generator);  // 随机角度


	// 计算偏移
	float radius = minRadius + (maxRadius - minRadius) * distribution(generator);
	float x = radius * sin(theta);
	float z = radius * cos(theta);

	// 应用偏移到原始坐标
	glm::vec3 randomPoint = center + glm::vec3(x, 0, z);

	return randomPoint;
}
enum upgrade_type {
	oneMoreTurret,
	IncreaseShotFrequency,
	IncreaseShipSpeed,
	crossBullet,
	oneMoreChance
};

class enemy
{
public:
	enemy(Node node, glm::vec3 direction, int type);
	enemy(Node node, glm::vec3 direction, int type, Node turret);
	~enemy();
	Node enemy_node;
	Node enemy_turret;
	bool isActive;
	glm::vec3 moveDirection;
	int enemy_type;
	float shotTimer = 0;

private:

};

enemy::enemy(Node node, glm::vec3 direction, int type)
{
	enemy_node = node;
	isActive = true;
	moveDirection = direction;
	enemy_type = type;
}
enemy::enemy(Node node, glm::vec3 direction, int type, Node turret)
{
	enemy_node = node;
	isActive = true;
	moveDirection = direction;
	enemy_type = type;
	enemy_turret = turret;
	enemy_node.add_child(&enemy_turret);
}

enemy::~enemy()
{

}
class bullet {
public:
	bullet(Node node, glm::vec3 direction);
	~bullet();
	Node bullet_node;
	bool isActive;
	glm::vec3 shot_direction;
};
bullet::bullet(Node node, glm::vec3 direction) {
	bullet_node = node;
	shot_direction = direction;
	isActive = true;
}
bullet::~bullet() {

}
class upgrade {
public:
	upgrade(Node node, upgrade_type Upgrade);
	~upgrade();
	Node upgrade_node;
	bool isActive;
	upgrade_type type;
};
upgrade::upgrade(Node node, upgrade_type Upgrade) {
	upgrade_node = node;
	isActive = true;
	type = Upgrade;
}
upgrade::~upgrade() {

}
upgrade* createUpgradeSphere(glm::vec3 Translation, std::vector<upgrade*>& upgrade_vector, upgrade_type type) {
	auto upgrade_shape = parametric_shapes::createSphere(1.5f, 10u, 10u);;
	Node upgrade_node;
	upgrade_node.set_geometry(upgrade_shape);
	upgrade_node.get_transform().SetTranslate(Translation);
	upgrade* new_upgrade = new upgrade(upgrade_node, type);
	upgrade_vector.push_back(new_upgrade);
	return new_upgrade;
}

enemy* createEnemy(int enemyType, glm::vec3 Translation, std::vector<enemy*>& enemy_vector, std::vector<bonobo::mesh_data> enemy_shape0, std::vector<bonobo::mesh_data> enemy_shape1, std::vector<bonobo::mesh_data> turret_shape) {
	if (!enemy_vector.empty()) {
		for (enemy* tmp_enemy : enemy_vector) {
			if (tmp_enemy->isActive == false && tmp_enemy->enemy_type == enemyType) {
				tmp_enemy->isActive = true;
				tmp_enemy->enemy_node.get_transform().SetTranslate(Translation);
				//std::cout << "reload existing boat" << std::endl;
				return tmp_enemy;
			}
		}
	}
	Node enemyTmp;
	Node turret;
	enemy* newEnemy = new enemy(enemyTmp, Translation, enemyType);
	if (enemyType == 1) {
		newEnemy->enemy_node.set_geometry(enemy_shape0[0]);
	}
	else {
		newEnemy->enemy_node.set_geometry(enemy_shape1[0]);
		turret.set_geometry(turret_shape[0]);
		newEnemy->enemy_turret = turret;
		newEnemy->enemy_turret.get_transform().SetTranslate(glm::vec3(0.0f, 1.5f, 1.5f));
		newEnemy->enemy_turret.get_transform().SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	}
	newEnemy->enemy_node.get_transform().SetTranslate(Translation);
	enemy_vector.push_back(newEnemy);
	//std::cout << "create new boat" << std::endl;
	return newEnemy;
}
bullet* createBullet(glm::vec3 Translation, glm::vec3 direction, std::vector<bullet*>& bullet_vector){
	
	if (!bullet_vector.empty()) {
		for (bullet* tmp_bullet : bullet_vector)
			if (tmp_bullet->isActive == false) {
				tmp_bullet->isActive = true;
				tmp_bullet->bullet_node.get_transform().SetTranslate(Translation);
				tmp_bullet->shot_direction = direction;
				return tmp_bullet;
			}
	}
	auto bullet_shape = parametric_shapes::createSphere(0.5f, 5u, 5u);;
	Node bullet_node;
	bullet_node.set_geometry(bullet_shape);
	bullet_node.get_transform().SetTranslate(Translation);
	bullet* new_bullet = new bullet(bullet_node, direction);
	bullet_vector.push_back(new_bullet);
	return new_bullet;
}

void
edaf80::Assignment5::run()
{
	// Set up the camera
	//mCamera.mWorld.SetTranslate(glm::vec3(0.0f, 0.0f, 6.0f));
	mCamera.mWorld.SetTranslate(glm::vec3(-40.0f, 14.0f, 6.0f));
	mCamera.mWorld.LookAt(glm::vec3(0.0f));
	mCamera.mMouseSensitivity = glm::vec2(0.003f);
	mCamera.mMovementSpeed = glm::vec3(15.0f); // 3 m/s => 10.8 km/h
	auto camera_position = mCamera.mWorld.GetTranslation();

	// Create the shader programs
	ShaderProgramManager program_manager;
	GLuint fallback_shader = 0u;
	program_manager.CreateAndRegisterProgram("Fallback",
		{ { ShaderType::vertex, "common/fallback.vert" },
		  { ShaderType::fragment, "common/fallback.frag" } },
		fallback_shader);
	if (fallback_shader == 0u) {
		LogError("Failed to load fallback shader");
		return;
	}
	GLuint diffuse_shader = 0u;
	program_manager.CreateAndRegisterProgram("Diffuse",
		{ { ShaderType::vertex, "EDAF80/diffuse.vert" },
		  { ShaderType::fragment, "EDAF80/diffuse.frag" } },
		diffuse_shader);
	if (diffuse_shader == 0u) {
		LogError("Failed to load diffuse shader");
		return;
	}
	GLuint water_shader = 0u;
	program_manager.CreateAndRegisterProgram("water",
		{ { ShaderType::vertex, "EDAF80/water.vert" },
		{ ShaderType::fragment, "EDAF80/water.frag" } },
		water_shader);
	if (water_shader == 0u) {
		LogError("Failed to load water shader");
		return;
	}
	GLuint skybox_shader = 0u;
	program_manager.CreateAndRegisterProgram("Skybox",
		{ { ShaderType::vertex, "EDAF80/skybox.vert" },
		{ ShaderType::fragment, "EDAF80/skybox.frag" } },
		skybox_shader);
	if (skybox_shader == 0u)
		LogError("Failed to load skybox shader");
	//GLuint phong_shader = 0u;
	//program_manager.CreateAndRegisterProgram("Phong",
	//	{ {ShaderType::vertex, "EDAF80/phong.vert"},
	//	{ShaderType::fragment, "EDAF80/phong.frag"} },
	//	phong_shader);
	//if (phong_shader == 0u)
	//	LogError("Failed to load phong shader");
	//
	// Todo: Insert the creation of other shader programs.
	//       (Check how it was done in assignment 3.)
	//

	//
	// Todo: Load your geometry added code
	//
	float elapsed_time_s = 0.0f;
	auto light_position = glm::vec3(-2.0f, 4.0f, 2.0f);
	//the base color of water
	glm::vec3 ambient_color = glm::vec3(0.2f, 0.2f, 0.6f);
	glm::vec4 deep_color = glm::vec4(0.0f, 0.0f, 0.1f, 1.0f);
	glm::vec4 shallow_color = glm::vec4(0.0f, 0.5f, 0.5f, 1.0f);
	//the attributes of waves
	float amplitude = 1.0f;
	float frequency = 0.2f;
	float amplitude1 = 0.5f;
	float frequency1 = 0.4f;
	float phare_constant = 0.5f;
	float phare_constant1 = 1.3f;
	float sharpness = 2.0f;
	float sharpness1 = 2.0f;
	glm::vec2 direction = glm::vec2(-1.0f, 0.0f);
	glm::vec2 direction1 = glm::vec2(-0.7f, 0.7f);

	glm::vec3 playerBoat_ambient_color = glm::vec3(0.1f, 0.6f, 0.0f);
	glm::vec3 playerTurret_ambient_color = glm::vec3(0.1f, 0.0f, 0.6f);
	glm::vec3 enemyBoat_ambient_color = glm::vec3(0.6f, 0.1f, 0.0f);
	glm::vec3 enemyTurret_ambient_color = glm::vec3(0.1f, 0.0f, 0.6f);
	glm::vec3 playerBullet_ambient_color = glm::vec3(0.2f, 0.2f, 0.2f);
	glm::vec3 enemyBullet_ambient_color = glm::vec3(0.6f, 0.1f, 0.1f);
	//the color of upgrade ball
	glm::vec3 upgrade_ambient_color = glm::vec3(0.0f, 1.0f, 0.0f);

	auto const playerboat_set_uniforms = [&light_position, &playerBoat_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(playerBoat_ambient_color));
		};
	auto const playerturret_set_uniforms = [&light_position, &playerTurret_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(playerTurret_ambient_color));
		};
	auto const enemyboat_set_uniforms = [&light_position, &enemyBoat_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(enemyBoat_ambient_color));
		};
	auto const enemyturret_set_uniforms = [&light_position, &enemyTurret_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(enemyTurret_ambient_color));
		};
	auto const playerbullet_set_uniforms = [&light_position, &playerBullet_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(playerBullet_ambient_color));
		};
	auto const enenmybullet_set_uniforms = [&light_position, &enemyBullet_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(enemyBullet_ambient_color));
		};
	auto const ungrade_set_uniforms = [&light_position, &upgrade_ambient_color](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(upgrade_ambient_color));
		};

	auto const water_set_uniforms = [&light_position, &camera_position, &ambient_color, &deep_color, &shallow_color, &elapsed_time_s, &amplitude, &frequency, &phare_constant, &sharpness, &direction, &amplitude1, &frequency1, &phare_constant1, &sharpness1, &direction1](GLuint program) {
		glUniform3fv(glGetUniformLocation(program, "light_position"), 1, glm::value_ptr(light_position));
		glUniform3fv(glGetUniformLocation(program, "camera_position"), 1, glm::value_ptr(camera_position));
		glUniform3fv(glGetUniformLocation(program, "ambient_color"), 1, glm::value_ptr(ambient_color));
		glUniform4fv(glGetUniformLocation(program, "deep_color"), 1, glm::value_ptr(deep_color));
		glUniform4fv(glGetUniformLocation(program, "shallow_color"), 1, glm::value_ptr(shallow_color));
		glUniform1f(glGetUniformLocation(program, "elapsed_time_s"), elapsed_time_s);
		glUniform1f(glGetUniformLocation(program, "amplitude"), amplitude);
		glUniform1f(glGetUniformLocation(program, "frequency"), frequency);
		glUniform1f(glGetUniformLocation(program, "phare_constant"), phare_constant);
		glUniform1f(glGetUniformLocation(program, "sharpness"), sharpness);
		glUniform2fv(glGetUniformLocation(program, "direction"), 1, glm::value_ptr(direction));
		glUniform1f(glGetUniformLocation(program, "amplitude1"), amplitude1);
		glUniform1f(glGetUniformLocation(program, "frequency1"), frequency1);
		glUniform1f(glGetUniformLocation(program, "phare_constant1"), phare_constant1);
		glUniform1f(glGetUniformLocation(program, "sharpness1"), sharpness1);
		glUniform2fv(glGetUniformLocation(program, "direction1"), 1, glm::value_ptr(direction1));
		};
	float waterWidth = 400.0f;
	float waterHeight = 400.0f;
	//load models
		auto enemy_shape0 = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/boat1.obj");
		auto enemy_shape1 = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/boat3.obj");
		auto turret_shape = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/turret3.obj");

		auto Player_shape = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/boat2.obj");
		Node Player;
		Player.set_geometry(Player_shape[0]);
		Player.set_program(&diffuse_shader, playerboat_set_uniforms);
		Player.get_transform().SetTranslate(glm::vec3(waterWidth / 2, 5.0f, waterHeight / 2));

		auto turret1_shape = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/turret2.obj");
		Node turret1;
		turret1.set_geometry(turret1_shape[0]);
		turret1.set_program(&diffuse_shader, playerturret_set_uniforms);

		auto turret2_shape = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/turret3.obj");
		Node turret2;
		turret2.set_geometry(turret2_shape[0]);
		turret2.set_program(&diffuse_shader, playerturret_set_uniforms);

		auto turret3_shape = bonobo::loadObjects("F:/desktop/CourseFile/ComputerGraphics/res/scenes/turret1.obj");
		Node turret3;
		turret3.set_geometry(turret3_shape[0]);
		turret3.set_program(&diffuse_shader, playerturret_set_uniforms);

		std::vector<Node> turret_vector;
		std::map<Node, bool> turret_Dictionary;
		Player.add_child(&turret1);
		turret1.get_transform().SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		turret1.get_transform().SetTranslate(glm::vec3(0, 1.2f, 2.0f));
		Player.add_child(&turret2);
		turret2.get_transform().SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		turret2.get_transform().SetTranslate(glm::vec3(0, 1.2f, -1.6f));
		Player.add_child(&turret3);
		turret3.get_transform().SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
		turret3.get_transform().SetTranslate(glm::vec3(0, 2.0f, 0.5f));

		//turret_Dictionary[turret1] = true;
		//turret_Dictionary[turret2] = false;
		//turret_Dictionary[turret3] = false;
		turret_vector.push_back(turret1);
		turret_vector.push_back(turret2);
		turret_vector.push_back(turret3);

		auto bullet_shape = parametric_shapes::createSphere(5.0f, 10u, 10u);
		//
		// Todo: Load your geometry
		//
		//set skybox
		auto skybox_shape = parametric_shapes::createSphere(20.0f, 100u, 100u);
		if (skybox_shape.vao == 0u) {
			LogError("Failed to retrieve the mesh for the skybox");
			return;
		}

		Node skybox;
		skybox.set_geometry(skybox_shape);

		GLuint cubemap = bonobo::loadTextureCubeMap(
			config::resources_path("cubemaps/NissiBeach2/posx.jpg"),
			config::resources_path("cubemaps/NissiBeach2/negx.jpg"),
			config::resources_path("cubemaps/NissiBeach2/posy.jpg"),
			config::resources_path("cubemaps/NissiBeach2/negy.jpg"),
			config::resources_path("cubemaps/NissiBeach2/posz.jpg"),
			config::resources_path("cubemaps/NissiBeach2/negz.jpg"));
		skybox.add_texture("cubemap", cubemap, GL_TEXTURE_CUBE_MAP);

		skybox.set_program(&skybox_shader, playerturret_set_uniforms);
		//set water quad

		auto water_shape = parametric_shapes::createQuad(waterWidth, waterHeight, 2000, 2000);

		if (water_shape.vao == 0u) {
			LogError("Failed to retrieve the mesh for the demo sphere");
			return;
		}

		Node water_quad;

		water_quad.set_geometry(water_shape);
		GLuint wave_texture = bonobo::loadTexture2D(config::resources_path("textures/waves.png"));
		water_quad.add_texture("waveTexture", wave_texture, GL_TEXTURE_2D);
		water_quad.add_texture("cubemap", cubemap, GL_TEXTURE_CUBE_MAP);
		water_quad.set_program(&water_shader, water_set_uniforms);
		//added end

		glClearDepthf(1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);


		auto lastTime = std::chrono::high_resolution_clock::now();

		bool pause_animation = false;
		bool use_orbit_camera = false;
		auto cull_mode = bonobo::cull_mode_t::disabled;
		auto polygon_mode = bonobo::polygon_mode_t::fill;

		bool show_logs = true;
		bool show_gui = true;
		bool shader_reload_failed = false;
		bool show_basis = false;
		float basis_thickness_scale = 1.0f;
		float basis_length_scale = 1.0f;

		changeCullMode(cull_mode);
		bool moveRight = false;
		bool moveDown = false;
		bool moveLeft = false;
		bool moveUp = false;

		bool isboatrotate = false;
		glm::vec3 moveDirection = glm::vec3(0);
		glm::vec3 rightDirection = glm::vec3(0);
		glm::vec3 leftDirection = glm::vec3(0);
		glm::vec3 upDirection = glm::vec3(0);
		glm::vec3 downDirection = glm::vec3(0);
		//player attribute
		float moveSpeed = 0.2;
		float playerBulletSpeed = 0.8f;
		glm::vec3 playerTranslation;
		float playerShotTimer = 0;
		float playerShotDuration = 1;
		float boatHitDistance = 3.0f;
		float bulletHitdistance = 2.0f;
		float playerAimDistance = 30.0f;
		int playerChance = 1;

		//upgrade choose
		int num_turret = 1;
		bool isBulletcross = false;

		float curBoatRoationAngle = 0.0f;

		bool isturrentrotate = false;
		float printTimer = 0;

		int count = 0;
		glm::vec3 cur_enemyLocation = glm::vec3(0, 0, 0);
		glm::vec3 cur_AmiedenemyLocation = glm::vec3(0, 0, 0);
		bool enemyFound = true;

		//store enemies and bullets
		std::vector<enemy*> enemies;
		std::vector<bullet*> player_bullets;
		std::vector<bullet*> enemies_bullets;
		std::vector<upgrade*> upgrades;
		float enemy1Speed = 0.05f;
		float enemy2Speed = 0.03f;
		float enemyBulletSpeed = 0.2f;
		float enemyShotDuration = 2;

		//gameController
		bool pauseGame = false;
		bool isAnyEnemyExist = false;
		float enemy1GenerationDuration = 1.5;
		float enemy2GenerationDuration = 3;
		float enemy1GenerationTimer = 0;
		float enemy2GenerationTimer = 0;
		int enemyKilled = 0;
		const char* shownText1 = "";
		const char* shownText2 = "";
		const char* shownText3 = "";
		int hardLevel = 0;

		playerTranslation = Player.get_transform().GetTranslation();
		//test enemy chase
		enemy* closet_enemy = createEnemy(1, glm::vec3(10, 0, -10), enemies, enemy_shape0, enemy_shape1, turret_shape);
		enemy* second_closet_enemy = closet_enemy;
		enemy* third_closet_enemy = closet_enemy;
		enemy* closest_enemies[] = { closet_enemy, second_closet_enemy, third_closet_enemy };

		enemy* tmpEnemy = createEnemy(1, getRandomPointInRadius(playerTranslation, 30.0, 50.0), enemies, enemy_shape0, enemy_shape1, turret_shape);
		enemy* tmpEnemy2 = createEnemy(2, getRandomPointInRadius(playerTranslation, 30.0, 50.0), enemies, enemy_shape0, enemy_shape1, turret_shape);
		tmpEnemy->enemy_node.set_program(&diffuse_shader, enemyboat_set_uniforms);
		tmpEnemy2->enemy_node.set_program(&diffuse_shader, enemyboat_set_uniforms);
		tmpEnemy2->enemy_turret.set_program(&diffuse_shader, enemyturret_set_uniforms);
		tmpEnemy->isActive = false;
		tmpEnemy2->isActive = false;
		enemies.push_back(tmpEnemy);
		enemies.push_back(tmpEnemy2);
	
		mCamera.mWorld.SetTranslate(playerTranslation + glm::vec3(0, 70.0f, 0));
		mCamera.mWorld.LookAt(playerTranslation, glm::vec3(1, 0, 0));

		//test
		int testCounter1 = 0;
		int testCounter2 = 0;

		while (!glfwWindowShouldClose(window)) {
			auto const nowTime = std::chrono::high_resolution_clock::now();
			auto const deltaTimeUs = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime);
			lastTime = nowTime;
			playerTranslation = Player.get_transform().GetTranslation();
			mCamera.mWorld.SetTranslate(playerTranslation + glm::vec3(-30.0f, 70.0f, 0));
			mCamera.mWorld.LookAt(playerTranslation, glm::vec3(1, 0, 0));
			if (!pause_animation) {
				elapsed_time_s += std::chrono::duration<float>(deltaTimeUs).count();
			}
			if (!pauseGame) {
				if (enemy1GenerationTimer < enemy1GenerationDuration) {
					enemy1GenerationTimer += std::chrono::duration<float>(deltaTimeUs).count();
				}
				else {
					enemy1GenerationTimer = 0;
					enemy* tmpEnemy = createEnemy(1, getRandomPointInRadius(playerTranslation, 30.0, 40.0), enemies, enemy_shape0, enemy_shape1, turret_shape);
					tmpEnemy->enemy_node.set_program(&diffuse_shader, enemyboat_set_uniforms);
					//enemies.push_back(tmpEnemy);
				}
				if (enemy2GenerationTimer < enemy2GenerationDuration) {
					enemy2GenerationTimer += std::chrono::duration<float>(deltaTimeUs).count();
				}
				else {
					enemy2GenerationTimer = 0;
					enemy* tmpEnemy = createEnemy(2, getRandomPointInRadius(playerTranslation, 30.0, 40.0), enemies, enemy_shape0, enemy_shape1, turret_shape);
					tmpEnemy->enemy_node.set_program(&diffuse_shader, enemyboat_set_uniforms);
					tmpEnemy->enemy_turret.set_program(&diffuse_shader, enemyturret_set_uniforms);
					//enemies.push_back(tmpEnemy);
				}
			}
			
			//turret1.get_transform().SetTranslate(Player.get_transform().GetTranslation() + glm::vec3(0, 1.2f, 2.0f));
			//turret2.get_transform().SetTranslate(Player.get_transform().GetTranslation() + glm::vec3(0, 1.2f, -1.6f));

			glm::vec3 move1(0, 0, 0.3);//right
			glm::vec3 move2(-0.3, 0, 0);//down
			glm::vec3 move3(0, 0, -0.3);//left
			glm::vec3 move4(0.3, 0, 0);//up
			if (!enemies.empty()) {
				bool tmp = false;
				for (enemy* enemy : enemies) {
					if (enemy->isActive == true) {
						tmp = true;
						break;
					}
				}
				if (tmp == true) {
					isAnyEnemyExist = true;
				}else
				{
					isAnyEnemyExist = false;
				}
			}
			if (moveRight || moveLeft || moveUp || moveDown) {
				glm::vec3 tmpDirection = rightDirection + leftDirection + upDirection + downDirection;
				if (tmpDirection.x == 0 && tmpDirection.z == 0) {
					moveDirection = tmpDirection;
				}
				else {
					moveDirection = glm::normalize(tmpDirection);
				}
				Player.get_transform().SetTranslate(Player.get_transform().GetTranslation() + moveDirection * moveSpeed);
				Player.get_transform().SetRotateY(rotateTurret(Player, moveDirection));

				float curRotationyCos = Player.get_transform().GetRotation()[0][0];
				float curRotationySin = Player.get_transform().GetRotation()[2][0];
				curBoatRoationAngle = judgeDirection(curRotationyCos, curRotationySin);
				//std::cout << "curBoatRoationAngle: " << curBoatRoationAngle << std::endl;
				//timerPrint("curBoatRoationAngle: ", curBoatRoationAngle, 1, printTimer, std::chrono::duration<float>(deltaTimeUs).count());
			}
			
			if (!enemies.empty()) {
				enemyFound = true;
				float shortestDistance = 10000.0f;
				float second_shortestDistance = 20000.0f;
				for (enemy* enemy : enemies) {
					if (enemy->isActive) {
						float distance = glm::distance(playerTranslation, enemy->enemy_node.get_transform().GetTranslation());
						if (distance < shortestDistance) {
							shortestDistance = distance;
							closest_enemies[2] = closest_enemies[1];
							closest_enemies[1] = closest_enemies[0];
							closest_enemies[0] = enemy;
							cur_enemyLocation = closet_enemy->enemy_node.get_transform().GetTranslation();
						}
						else if (distance < second_shortestDistance) {
							second_shortestDistance = distance;
							closest_enemies[2] = closest_enemies[1];
							closest_enemies[1] = enemy;
						}
					}
				}
				if (isAnyEnemyExist) {
					if (playerShotTimer < playerShotDuration) {
						playerShotTimer += std::chrono::duration<float>(deltaTimeUs).count();
					}
					else {
						playerShotTimer = 0;
						for (int i = 0; i < num_turret; i++) {
							glm::mat4x4 translate_Matrix = Player.get_transform().GetMatrix() * turret_vector[i].get_transform().GetTranslationMatrix();
							glm::vec3 turret_location = glm::vec3(translate_Matrix[3][0], translate_Matrix[3][1], translate_Matrix[3][2]);
							glm::vec3 shotDirection = glm::normalize(closest_enemies[i]->enemy_node.get_transform().GetTranslation() - turret_location);
							bullet* tmpBullet = createBullet(turret_location, shotDirection, player_bullets);
							tmpBullet->bullet_node.set_program(&diffuse_shader, playerbullet_set_uniforms);
							//std::cout << enemies.size()  << std::endl;
						}
						//glm::vec3 shotDirection = glm::normalize(cur_enemyLocation - playerTranslation);
						//bullet* tmpBullet = createBullet(playerTranslation, shotDirection, player_bullets);
						//tmpBullet->bullet_node.set_program(&diffuse_shader, set_uniforms);
					}
				}
				else {
					enemyFound = false;
				}
			}	
			if (enemyFound) {
				//glm::vec3 turretDirection1 = glm::normalize(cur_enemyLocation - Player.get_transform().GetTranslation());
				//turret1.get_transform().LookAt(enemyLocation, glm::uvec3(0, 1, 0));
				//turret1.get_transform().SetRotateY(rotateTurret(turret1, turretDirection1 , curBoatRoationAngle));
				for (int i = 0; i < num_turret; i++) {
					glm::vec3 turretDirection = glm::normalize(closest_enemies[i]->enemy_node.get_transform().GetTranslation() - Player.get_transform().GetTranslation());
					turret_vector[i].get_transform().SetRotateY(rotateTurret(turret_vector[i], turretDirection, curBoatRoationAngle));
				}
			}
			
			if (!pauseGame) {
				if (!enemies.empty()) {
					for (enemy* enemy : enemies) {
						if (enemy->isActive == true) {
							glm::vec3 enemyLocation = enemy->enemy_node.get_transform().GetTranslation();
							glm::vec3 chaseDirection = playerTranslation - enemyLocation;
							if (chaseDirection.x != 0 || chaseDirection.z != 0) {
								chaseDirection = glm::normalize(chaseDirection);
							}
							if (enemy->enemy_type == 1) {
								enemy->enemy_node.get_transform().SetTranslate(enemyLocation + chaseDirection * enemy1Speed);
							}
							else if (enemy->enemy_type == 2) {
								enemy->enemy_node.get_transform().SetTranslate(enemyLocation + chaseDirection * enemy2Speed);
							}
							enemy->enemy_node.get_transform().SetRotateY(rotateTurret(enemy->enemy_node, chaseDirection));
							if (enemy->enemy_type == 2) {
								float curRotationyCos = enemy->enemy_node.get_transform().GetRotation()[0][0];
								float curRotationySin = enemy->enemy_node.get_transform().GetRotation()[2][0];
								float curEnemyRoationAngle = judgeDirection(curRotationyCos, curRotationySin);
								glm::vec3 turretDirection = glm::normalize(Player.get_transform().GetTranslation() - enemyLocation);
								enemy->enemy_turret.get_transform().SetRotateY(rotateTurret(enemy->enemy_turret, turretDirection, curEnemyRoationAngle));
								//enemy->enemy_turret.get_transform().LookAt(playerTranslation, glm::uvec3(0, 1, 0));
							}
						}
					}
				}
				if (!player_bullets.empty()) {
					for (bullet* Bullet : player_bullets) {
						if (Bullet->isActive) {
							glm::vec3 bullet_location = Bullet->bullet_node.get_transform().GetTranslation();
							Bullet->bullet_node.get_transform().SetTranslate(bullet_location + Bullet->shot_direction * playerBulletSpeed);
							
						}
					}
				}
				if (!enemies_bullets.empty()) {
					for (bullet* Bullet : enemies_bullets) {
						if (Bullet->isActive) {
							glm::vec3 bullet_location = Bullet->bullet_node.get_transform().GetTranslation();
							Bullet->bullet_node.get_transform().SetTranslate(bullet_location + Bullet->shot_direction * enemyBulletSpeed);
						}
					}
				}
				if (!enemies.empty()) {
					for (enemy* enemy : enemies) {
						if (enemy->isActive && enemy->enemy_type == 2) {
							if (enemy->shotTimer < enemyShotDuration) {
								enemy->shotTimer += std::chrono::duration<float>(deltaTimeUs).count();
							}
							else {
								enemy->shotTimer = 0;
								glm::vec3 shotDirection = glm::normalize(playerTranslation - enemy->enemy_node.get_transform().GetTranslation());
								bullet* tmpBullet = createBullet(enemy->enemy_node.get_transform().GetTranslation(), shotDirection, enemies_bullets);
								tmpBullet->bullet_node.set_program(&diffuse_shader, enemyboat_set_uniforms);
							}
						}
					}
				}
				if (!player_bullets.empty() && !enemies.empty()) {
					for (bullet* bullet : player_bullets) {
						if (bullet->isActive) {
							for (enemy* enemy : enemies) {
								if (enemy->isActive) {
									if (glm::distance(bullet->bullet_node.get_transform().GetTranslation(), enemy->enemy_node.get_transform().GetTranslation()) < bulletHitdistance) {
										if (!isBulletcross) {
											bullet->isActive = false;
										}
										enemy->isActive = false;
										enemyKilled++;
										//std::cout << "current enemy vector size: "<<enemies.size() << std::endl;
									}
								}
							}
						}
					}
				}
				if (!enemies_bullets.empty()) {
					for (bullet* bullet : enemies_bullets) {
						if (bullet->isActive) {
							if (glm::distance(bullet->bullet_node.get_transform().GetTranslation(), playerTranslation) < bulletHitdistance) {
								playerChance--;
								bullet->isActive = false;
								if (playerChance == 0) {
									pauseGame = true;
									shownText2 = "game over, your final score is: %d";
								}
							}
						}
					}
				}
				if (!upgrades.empty()) {
					for (upgrade* upgrade : upgrades) {
						if (upgrade->isActive) {
							if (glm::distance(upgrade->upgrade_node.get_transform().GetTranslation(), playerTranslation) < boatHitDistance) {
								upgrade->isActive = false;
								switch (upgrade->type) {
								case upgrade_type(oneMoreTurret):
									num_turret++;
									shownText1 = "one more turret!";
									break;
								case upgrade_type(IncreaseShotFrequency):
									playerShotDuration = playerShotDuration / 2;
									shownText1 = "you can shot faster!";
									break;
								case upgrade_type(IncreaseShipSpeed):
									playerBulletSpeed = 1.5 * playerBulletSpeed;
									shownText1 = "now you can move faster!";
									break;
								case upgrade_type(crossBullet):
									isBulletcross = true;
									shownText1 = "the bullet can cross the enemies!";
									break;
								case upgrade_type(oneMoreChance):
									playerChance++;
									shownText1 = "you get one more chance";
									break;
								}
								//std::cout << "game over" << std::endl;
							}
						}
					}
				}
				if (!player_bullets.empty()) {
					for (bullet* bullet : player_bullets) {
						if (glm::distance(bullet->bullet_node.get_transform().GetTranslation(), playerTranslation) > 1000) {
							bullet->isActive = false;
						}
					}
				}
				if (!enemies_bullets.empty()) {
					for (bullet* bullet : enemies_bullets) {
						if (glm::distance(bullet->bullet_node.get_transform().GetTranslation(), playerTranslation) > 1000) {
							bullet->isActive = false;
						}
					}
				}
				if (isAnyEnemyExist) {
					for (enemy* enemy : enemies) {
						if (enemy->isActive) {
							if (glm::distance(enemy->enemy_node.get_transform().GetTranslation(), playerTranslation) < boatHitDistance) {
								playerChance--;
								enemy->isActive = false;
								if (playerChance == 0) {
									pauseGame = true;
									shownText2 = "game over, your final score is: %d";
								}
							}
						}
					}
				}
			}
			if (enemyKilled == 20 && hardLevel == 0) {
				enemy1GenerationDuration = 1.5;//0.6;
				enemy2GenerationDuration = 3;
				enemy1Speed = 0.08;
				upgrade* tmpupgrade = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreChance));
				tmpupgrade->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				upgrade* tmpupgrade1 = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(IncreaseShipSpeed));
				tmpupgrade1->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				shownText1 = "hard level increase \n get you upgrade by touching the Green Shpere";
				hardLevel++;
			}
			else if (enemyKilled == 40 && hardLevel == 1) {
				enemy1GenerationDuration = 1.0;
				enemy2GenerationDuration = 2.0;
				enemy2Speed = 0.05;
				upgrade* tmpupgrade = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreChance));
				tmpupgrade->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				upgrade* tmpupgrade1 = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreTurret));
				tmpupgrade1->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				shownText1 = "hard level increase again\n the upgrade can help you kill enemies";
				hardLevel++;
			}
			else if (enemyKilled == 80 && hardLevel == 2) {
				enemy1GenerationDuration = 0.6;
				enemy2GenerationDuration = 1.5;
				upgrade* tmpupgrade = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreChance));
				tmpupgrade->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				upgrade* tmpupgrade1 = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreTurret));
				tmpupgrade1->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				shownText1 = "hard level increase again, upgrades updated";
				hardLevel++;
			}
			else if (enemyKilled == 140 && hardLevel == 3) {
				enemy1GenerationDuration = 0.4;
				enemy2GenerationDuration = 1.0;
				upgrade* tmpupgrade = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreChance));
				tmpupgrade->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				upgrade* tmpupgrade1 = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(IncreaseShotFrequency));
				tmpupgrade1->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				shownText1 = "hard level increase to level4, upgrades updated";
				hardLevel++;
			}
			else if (enemyKilled == 200 && hardLevel == 4) {
				enemy1GenerationDuration = 0.2;
				enemy2GenerationDuration = 0.6;
				upgrade* tmpupgrade = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(oneMoreChance));
				tmpupgrade->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				upgrade* tmpupgrade1 = createUpgradeSphere(getRandomPointInRadius(playerTranslation, 20.0, 30.0), upgrades, upgrade_type(crossBullet));
				tmpupgrade1->upgrade_node.set_program(&diffuse_shader, ungrade_set_uniforms);
				shownText1 = "hard level now has increased to to the largest level\n check out how long you can continue playing";
				hardLevel++;
			}



			auto& io = ImGui::GetIO();
			inputHandler.SetUICapture(io.WantCaptureMouse, io.WantCaptureKeyboard);

			glfwPollEvents();
			inputHandler.Advance();
			mCamera.Update(deltaTimeUs, inputHandler);
			if (use_orbit_camera) {
				mCamera.mWorld.LookAt(glm::vec3(0.0f));
			}
			
			camera_position = mCamera.mWorld.GetTranslation();

			if (inputHandler.GetKeycodeState(GLFW_KEY_R) & JUST_PRESSED) {
				shader_reload_failed = !program_manager.ReloadAllPrograms();
				if (shader_reload_failed)
					tinyfd_notifyPopup("Shader Program Reload Error",
						"An error occurred while reloading shader programs; see the logs for details.\n"
						"Rendering is suspended until the issue is solved. Once fixed, just reload the shaders again.",
						"error");
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_F3) & JUST_RELEASED)
				show_logs = !show_logs;
			if (inputHandler.GetKeycodeState(GLFW_KEY_F2) & JUST_RELEASED)
				show_gui = !show_gui;
			if (inputHandler.GetKeycodeState(GLFW_KEY_F11) & JUST_RELEASED)
				mWindowManager.ToggleFullscreenStatusForWindow(window);


			// Retrieve the actual framebuffer size: for HiDPI monitors,
			// you might end up with a framebuffer larger than what you
			// actually asked for. For example, if you ask for a 1920x1080
			// framebuffer, you might get a 3840x2160 one instead.
			// Also it might change as the user drags the window between
			// monitors with different DPIs, or if the fullscreen status is
			// being toggled.
			int framebuffer_width, framebuffer_height;
			glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
			glViewport(0, 0, framebuffer_width, framebuffer_height);


			//
			// Todo: If you need to handle inputs, you can do it here
			//
			if (inputHandler.GetKeycodeState(GLFW_KEY_L) & JUST_PRESSED) {
				moveRight = true;
				isboatrotate = true;
				rightDirection = glm::vec3(0,0,1);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_L) & JUST_RELEASED) {
				moveRight = false;
				isboatrotate = false;
				rightDirection = glm::vec3(0,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_K) & JUST_PRESSED) {
				moveDown = true;
				isboatrotate = true;
				downDirection = glm::vec3(-1,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_K) & JUST_RELEASED) {
				moveDown = false;
				isboatrotate = false;
				downDirection = glm::vec3(0,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_J) & JUST_PRESSED) {
				moveLeft = true;
				isboatrotate = true;
				leftDirection = glm::vec3(0,0,-1);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_J) & JUST_RELEASED) {
				moveLeft = false;
				isboatrotate = false;
				leftDirection = glm::vec3(0,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_I) & JUST_PRESSED) {
				moveUp = true;
				isboatrotate = true;
				upDirection = glm::vec3(1,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_I) & JUST_RELEASED) {
				moveUp = false;
				isboatrotate = false;
				upDirection = glm::vec3(0,0,0);
			}
			if (inputHandler.GetKeycodeState(GLFW_KEY_T) & JUST_PRESSED) {
				if (pauseGame == false) {
					pauseGame = true;
				}else
				{
					pauseGame = false;
				}
				
				if (count == 0) {
					//cur_enemyLocation = enemyLocation1;
					//turret2.get_transform().RotateY(1.5);
					count++;
				}
				else if (count == 1) {
					//cur_enemyLocation = enemyLocation2;
					//turret2.get_transform().RotateY(4.7);
					count++;
				}
				else if (count == 2) {
					//cur_enemyLocation = enemyLocation3;
					//turret2.get_transform().RotateY(5.5);
					count = 0;
				}
			}


			mWindowManager.NewImGuiFrame();

			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			bonobo::changePolygonMode(polygon_mode);


			if (!shader_reload_failed) {
				//
				// Todo: Render all your geometry here.
				//
				skybox.get_transform().SetTranslate(camera_position);
				glDisable(GL_DEPTH_TEST);
				skybox.render(mCamera.GetWorldToClipMatrix());
				glEnable(GL_DEPTH_TEST);
				water_quad.render(mCamera.GetWorldToClipMatrix());
				Player.render(mCamera.GetWorldToClipMatrix());
				//turret1.render(mCamera.GetWorldToClipMatrix(), Player.get_transform().GetMatrix());
				//turret2.render(mCamera.GetWorldToClipMatrix(), Player.get_transform().GetMatrix());
				for (int i = 0; i < num_turret; i++) {
					turret_vector[i].render(mCamera.GetWorldToClipMatrix(), Player.get_transform().GetMatrix());
				}
				for (enemy* enemy : enemies) {
					if (enemy->isActive) {						
						if (enemy->enemy_type == 1) {
							enemy->enemy_node.render(mCamera.GetWorldToClipMatrix());
						}
						else {
							enemy->enemy_node.render(mCamera.GetWorldToClipMatrix());
							//std::cout << enemy->enemy_turret.get_transform().GetTranslation() << std::endl;
							enemy->enemy_turret.render(mCamera.GetWorldToClipMatrix(), enemy->enemy_node.get_transform().GetMatrix());
						}
					}
				}
				for (bullet* Bullet : player_bullets) {
					if (Bullet->isActive) {
						Bullet->bullet_node.render(mCamera.GetWorldToClipMatrix());
					}
				}
				for (bullet* Bullet : enemies_bullets) {
					if (Bullet->isActive) {
						Bullet->bullet_node.render(mCamera.GetWorldToClipMatrix());
					}
				}
				for (upgrade* upgrade : upgrades) {
					if (upgrade->isActive) {
						upgrade->upgrade_node.render(mCamera.GetWorldToClipMatrix());
					}
				}
			}


			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			//
			// Todo: If you want a custom ImGUI window, you can set it up
			//       here
			//
			ImGui::SetNextWindowPos(ImVec2(0, 0)); // 设置下一个窗口的位置
			ImGui::SetWindowFontScale(2.0f); // 设置字体缩放比例为1.5倍

			// 显示带颜色的文本
			ImVec4 textColor(1.0f, 1.0f, 1.0f, 1.0f); 
			ImGui::TextColored(textColor, "The enemy you killed: %d", enemyKilled);
			ImVec4 textColor1(0.0f, 1.0f, 0.0f, 1.0f); 			
			ImGui::TextColored(textColor, "The chance you left: %d", playerChance);
			ImGui::TextColored(textColor, shownText1);
			ImVec4 textColor2(1.0f, 0.0f, 0.0f, 1.0f); // 红色
			ImGui::TextColored(textColor2, shownText2, enemyKilled);

			//ImGui::Text("This is a text test");
			//bool const opened = ImGui::Begin("Scene Controls", nullptr, ImGuiWindowFlags_None);
			//if (opened) {
				//ImGui::Text("This is a text test");
				/*ImGui::Checkbox("Pause animation", &pause_animation);
				ImGui::Checkbox("Use orbit camera", &use_orbit_camera);
				ImGui::Separator();
				auto const cull_mode_changed = bonobo::uiSelectCullMode("Cull mode", cull_mode);
				if (cull_mode_changed) {
					changeCullMode(cull_mode);
				}
				bonobo::uiSelectPolygonMode("Polygon mode", polygon_mode);
				ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient_color));
				ImGui::ColorEdit3("deep_color", glm::value_ptr(deep_color));
				ImGui::ColorEdit3("shallow_color", glm::value_ptr(shallow_color));

				ImGui::SliderFloat("amplitude", &amplitude, 0.1f, 10.0f);
				ImGui::SliderFloat("frequency", &frequency, 1.0f, 100.0f);
				ImGui::SliderFloat("phare_constant", &phare_constant, 1.0f, 100.0f);
				ImGui::SliderFloat("sharpness", &sharpness, 1.0f, 100.0f);
				ImGui::SliderFloat("direction.x", &direction.x, -1.0f, 1.0f);
				ImGui::SliderFloat("direction.y", &direction.y, -1.0f, 1.0f);

				ImGui::SliderFloat("amplitude1", &amplitude1, 0.1f, 10.0f);
				ImGui::SliderFloat("frequency1", &frequency1, 1.0f, 100.0f);
				ImGui::SliderFloat("phare_constant1", &phare_constant1, 1.0f, 100.0f);
				ImGui::SliderFloat("sharpness1", &sharpness1, 1.0f, 100.0f);
				ImGui::SliderFloat("direction1.x", &direction1.x, -1.0f, 1.0f);
				ImGui::SliderFloat("direction1.y", &direction1.y, -1.0f, 1.0f);
				ImGui::SliderFloat3("Light Position", glm::value_ptr(light_position), -200.0f, 200.0f);

				ImGui::Separator();
				ImGui::Checkbox("Show basis", &show_basis);
				ImGui::SliderFloat("Basis thickness scale", &basis_thickness_scale, 0.0f, 100.0f);
				ImGui::SliderFloat("Basis length scale", &basis_length_scale, 0.0f, 100.0f);*/
			//}
			//ImGui::End();

			if (show_basis)
				bonobo::renderBasis(basis_thickness_scale, basis_length_scale, mCamera.GetWorldToClipMatrix());
			//if (show_logs)
			//	Log::View::Render();
			mWindowManager.RenderImGuiFrame(show_gui);

			glfwSwapBuffers(window);
		}
	}


int main()
{
	std::setlocale(LC_ALL, "");

	Bonobo framework;

	try {
		edaf80::Assignment5 assignment5(framework.GetWindowManager());
		assignment5.run();
	} catch (std::runtime_error const& e) {
		LogError(e.what());
	}
}
