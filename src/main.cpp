#include "common.h"
#include "Image.h"
#include "Player.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <string.h>
#include <fstream>
#include <vector>
#include <string>

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

std::vector<std::vector<char>> map(4, std::vector<char>(5));
std::vector<std::vector<char>> room_map(32, std::vector<char>(32));
bool new_room = true, lock_door;
int movement = -1;

void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		break;
	case GLFW_KEY_1:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case GLFW_KEY_2:
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	default:
		if (action == GLFW_PRESS)
		{
			Input.keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			Input.keys[key] = false;
		}
	}
}

void rerenderMapKey(Image& screenBuffer, const Point& coords, Image& new_tile)
{
	for (int i = 0; i < 32; ++i)
	{
		memcpy(screenBuffer.Data() + screenBuffer.Width() * coords.y + (31 - i) * screenBuffer.Width() + coords.x, new_tile.Data() + i * tileSize, tileSize * 4);
	}
}

int processPlayerMovement(Player& player, Image& screenBuffer, Image& new_tile)
{
	bool death_flag = false;
	bool change_room = false;
	bool reach_exit = false;
	Point room_coords = player.GetRoomCoords();
	movement = -1;
	if (Input.keys[GLFW_KEY_W])
	{
		if (room_map[room_coords.x - 1][room_coords.y] == '.')
		{
			//player.ProcessInput(MovementDir::UP);
			movement = 10;
		}
		else if (room_map[room_coords.x - 1][room_coords.y] == ' ')
		{
			//player.ProcessInput(MovementDir::UP);
			death_flag = true;
			movement = 10;
		}
		else if (room_map[room_coords.x - 1][room_coords.y] == 'x')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::UP);
				change_room = true;
				new_room = true;
				--player.room.x;
				movement = 10;
			}
		}
		else if (room_map[room_coords.x - 1][room_coords.y] == 'Q')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::UP);
				reach_exit = true;
				movement = 10;
			}
		}
		else if (room_map[room_coords.x - 1][room_coords.y] == 'K')
		{
			//player.ProcessInput(MovementDir::UP);
			player.have_key = true;
			rerenderMapKey(screenBuffer, { player.coords.x, player.coords.y + 32 }, new_tile);
			movement = 10;
		}
	}
	else if (Input.keys[GLFW_KEY_S])
	{
		if (room_map[room_coords.x + 1][room_coords.y] == '.')
		{
			//player.ProcessInput(MovementDir::DOWN);
			movement = 11;
		}
		else if (room_map[room_coords.x + 1][room_coords.y] == ' ')
		{
			//player.ProcessInput(MovementDir::DOWN);
			death_flag = true;
			movement = 11;
		}
		else if (room_map[room_coords.x + 1][room_coords.y] == 'x')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::DOWN);
				change_room = true;
				new_room = true;
				++player.room.x;
				movement = 11;
			}
		}
		else if (room_map[room_coords.x + 1][room_coords.y] == 'Q')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::DOWN);
				reach_exit = true;
				movement = 11;
			}
		}
		else if (room_map[room_coords.x + 1][room_coords.y] == 'K')
		{
			//player.ProcessInput(MovementDir::DOWN);
			player.have_key = true;
			rerenderMapKey(screenBuffer, { player.coords.x, player.coords.y - 32 }, new_tile);
			movement = 11;
		}
	}
	else if (Input.keys[GLFW_KEY_A])
	{
		if (room_map[room_coords.x][room_coords.y - 1] == '.')
		{
			//player.ProcessInput(MovementDir::LEFT);
			movement = 12;
		}
		else if (room_map[room_coords.x][room_coords.y - 1] == ' ')
		{
			//player.ProcessInput(MovementDir::LEFT);
			death_flag = true;
			movement = 12;
		}
		else if (room_map[room_coords.x][room_coords.y - 1] == 'x')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::LEFT);
				change_room = true;
				new_room = true;
				--player.room.y;
				movement = 12;
			}
		}
		else if (room_map[room_coords.x][room_coords.y - 1] == 'Q')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::LEFT);
				reach_exit = true;
				movement = 12;
			}
		}
		else if (room_map[room_coords.x][room_coords.y - 1] == 'K')
		{
			//player.ProcessInput(MovementDir::LEFT);
			player.have_key = true;
			rerenderMapKey(screenBuffer, { player.coords.x - 32, player.coords.y }, new_tile);
			movement = 12;
		}
	}
	else if (Input.keys[GLFW_KEY_D])
	{
		if (room_map[room_coords.x][room_coords.y + 1] == '.')
		{
			//player.ProcessInput(MovementDir::RIGHT);
			movement = 13;
		}
		else if (room_map[room_coords.x][room_coords.y + 1] == ' ')
		{
			//player.ProcessInput(MovementDir::RIGHT);
			death_flag = true;
			movement = 13;
		}
		else if (room_map[room_coords.x][room_coords.y + 1] == 'x')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::RIGHT);
				change_room = true;
				new_room = true;
				++player.room.y;
				movement = 13;
			}
		}
		else if (room_map[room_coords.x][room_coords.y + 1] == 'Q')
		{
			if (!lock_door || (lock_door && player.have_key))
			{
				//player.ProcessInput(MovementDir::RIGHT);
				reach_exit = true;
				movement = 13;
			}
		}
		else if (room_map[room_coords.x][room_coords.y + 1] == 'K')
		{
			//player.ProcessInput(MovementDir::RIGHT);
			player.have_key = true;
			rerenderMapKey(screenBuffer, { player.coords.x + 32, player.coords.y }, new_tile);
			movement = 13;
		}
	}

	if (death_flag)
	{
		return -1;
	}
	else if (change_room)
	{
		return 0;
	}
	else if (reach_exit)
	{
		return 1;
	}
	else
	{
		return -2;
	}
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		Input.captureMouse = !Input.captureMouse;
	}
	if (Input.captureMouse)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		Input.capturedMouseJustNow = true;
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (Input.firstMouse)
	{
		Input.lastX = float(xpos);
		Input.lastY = float(ypos);
		Input.firstMouse = false;
	}
	GLfloat xoffset = float(xpos) - Input.lastX;
	GLfloat yoffset = Input.lastY - float(ypos);
	Input.lastX = float(xpos);
	Input.lastY = float(ypos);
}

void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}

int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	std::cout << "Controls:"<< std::endl;
	std::cout << "press right mouse button to capture/release mouse cursor"<< std::endl;
	std::cout << "W, A, S, D - movement"<< std::endl;
	std::cout << "press ESC to exit" << std::endl;
	return 0;
}

int main(int argc, char** argv)
{
	if (!glfwInit())
	{
		return -1;
	}

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 var B2", nullptr, nullptr);
	
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, OnKeyboardPressed);  
	glfwSetCursorPosCallback(window, OnMouseMove); 
	glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback(window, OnMouseScroll);

	if (initGL() != 0)
	{
		return -1;
	}
	
	// Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
	{
		gl_error = glGetError();
	}

	Point starting_pos{ .x = 32, .y = 960 };
	Image img_door("./../resources/door.png");
	Image img_exit("./../resources/exit.png");
	Image img_grass("./../resources/grass.png");
	Image img_hole("./../resources/hole.png");
	Image img_wall("./../resources/wall.png");
	Image img_key("./../resources/key.png");
	Image win("./../resources/win.png");
	Image game_over("./../resources/game_over.png");
	Image doomguy("./../resources/doomguy.png");
	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 3);
	Image full_map(WINDOW_WIDTH, WINDOW_HEIGHT, 3);
	Image full_map_tmp(WINDOW_WIDTH, WINDOW_HEIGHT, 3);
	Player player(starting_pos, doomguy);

	std::ifstream fin("./../resources/main_map.txt", std::ios_base::in);
	std::string str;
	for (int i = 0; i < 4; ++i)
	{
		fin >> str;
		for (int j = 0; j < 5; ++j)
		{
			map[i][j] = str[j];
		}
	}
	fin.close();

	//std::ofstream fout("./../resources/logfile.txt", std::ios_base::out);

	char cur_room;
	bool win_flag = false, death_flag = false, first_time = true;
	
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

	// game loop
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		if (death_flag)
		{
			for (int i = 0; i < 1024; ++i)
			{
				memcpy(screenBuffer.Data() + (1023 - i) * 1024, game_over.Data() + i * 1024, 1024 * 4);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
			glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
			glfwSwapBuffers(window);
			continue;
		}
		if (win_flag)
		{
			for (int i = 0; i < 1024; ++i)
			{
				memcpy(screenBuffer.Data() + (1023 - i) * 1024, win.Data() + i * 1024, 1024 * 4);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
			glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
			glfwSwapBuffers(window);
			continue;
		}
		if (new_room)
		{
			new_room = false;
			lock_door = false;
			cur_room = map[player.GetRoom().x][player.GetRoom().y];
			switch (cur_room)
			{
			case 'A':
				fin.open("./../resources/map_a.txt", std::ios_base::in);
				break;
			case 'B':
				fin.open("./../resources/map_b.txt", std::ios_base::in);
				break;
			case 'C':
				fin.open("./../resources/map_c.txt", std::ios_base::in);
				break;
			case 'D':
				fin.open("./../resources/map_d.txt", std::ios_base::in);
				break;
			case 'E':
				fin.open("./../resources/map_e.txt", std::ios_base::in);
				break;
			case 'F':
				fin.open("./../resources/map_f.txt", std::ios_base::in);
				break;
			case 'G':
				fin.open("./../resources/map_g.txt", std::ios_base::in);
				break;
			case 'H':
				fin.open("./../resources/map_h.txt", std::ios_base::in);
				break;
			case 'K':
				fin.open("./../resources/map_k.txt", std::ios_base::in);
				break;
			}

			for (unsigned long long i = 0; i < 32; ++i)
			{
				std::getline(fin, str);
				for (unsigned long long j = 0; j < 32; ++j)
				{
					if (str[j] == '#')
					{
						room_map[i][j] = '#';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_wall.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
					else if (str[j] == '.')
					{
						room_map[i][j] = '.';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_grass.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
					else if (str[j] == ' ')
					{
						room_map[i][j] = ' ';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_hole.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
					else if (str[j] == 'x')
					{
						room_map[i][j] = 'x';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_door.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
					else if (str[j] == 'Q')
					{
						room_map[i][j] = 'Q';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_exit.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
					else
					{
						lock_door = true;
						room_map[i][j] = 'K';
						for (int k = 0; k < tileSize; ++k)
						{
							memcpy(full_map.Data() + (31 - i) * 1024 * tileSize + j * tileSize + k * 1024, img_key.Data() + (tileSize - 1 - k) * tileSize, tileSize * 4);
						}
					}
				}
			}
			fin.close();
			if (first_time)
			{
				first_time = false;
				memcpy(screenBuffer.Data(), full_map.Data(), 1024 * 1024 * 4);
				player.coords = starting_pos;
				player.old_coords = starting_pos;
				player.room_coords = Point{ 1, 1 };
				player.have_key = false;
				player.Draw(screenBuffer, full_map);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
				glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
				glfwSwapBuffers(window);
			}
			else
			{
				for (int i = 0; i < 1023; ++i)
				{
					for (int j = 0; j < 1024; ++j)
					{
						for (int k = i + 1; k < 1024; ++k)
						{
							full_map_tmp.PutPixel(k - 1 - i, j, screenBuffer.GetPixel(k, j));
						}
						for (int k = 0; k < i + 1; ++k)
						{
							full_map_tmp.PutPixel(1023 - i + k, j, full_map.GetPixel(k, j));
						}
					}
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
					glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, full_map_tmp.Data()); GL_CHECK_ERRORS;
					glfwSwapBuffers(window);
				}
				memcpy(screenBuffer.Data(), full_map.Data(), 1024 * 1024 * 4);
				player.coords = starting_pos;
				player.old_coords = starting_pos;
				player.room_coords = Point{ 1, 1 };
				player.have_key = false;
				player.Draw(screenBuffer, full_map);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
				glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
				glfwSwapBuffers(window);
			}
		}

		int move_res = processPlayerMovement(player, full_map, img_grass);
		if (move_res == -1)
		{
			death_flag = true;
		}
		else if (move_res == 1)
		{
			win_flag = true;
		}

		if (movement != -1)
		{
			for (int i = 0; i < 16; ++i)
			{
				if (movement == 10)
				{
					player.ProcessInput(MovementDir::UP);
					player.Draw(screenBuffer, full_map);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
					glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
					glfwSwapBuffers(window);
				}
				else if (movement == 11)
				{
					player.ProcessInput(MovementDir::DOWN);
					player.Draw(screenBuffer, full_map);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
					glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
					glfwSwapBuffers(window);
				}
				else if (movement == 12)
				{
					player.ProcessInput(MovementDir::LEFT);
					player.Draw(screenBuffer, full_map);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
					glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
					glfwSwapBuffers(window);
				}
				else
				{
					player.ProcessInput(MovementDir::RIGHT);
					player.Draw(screenBuffer, full_map);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
					glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
					glfwSwapBuffers(window);
				}
			}
			if (movement == 10)
			{
				--player.room_coords.x;
			}
			else if (movement == 11)
			{
				++player.room_coords.x;
			}
			else if (movement == 12)
			{
				--player.room_coords.y;
			}
			else
			{
				++player.room_coords.y;
			}
		}
	}

	glfwTerminate();
	return 0;
}
