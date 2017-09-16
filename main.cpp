#include "load_save_png.hpp"
#include "GL.hpp"

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <chrono>
#include <iostream>
#include <stdexcept>

static GLuint compile_shader(GLenum type, std::string const &source);
static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader);

int main(int argc, char **argv) {
	//Configuration:
	struct {
		std::string title = "Game1: Text/Tiles";
		glm::uvec2 size = glm::uvec2(640, 640);
	} config;

	//------------  initialization ------------

	//Initialize SDL library:
	SDL_Init(SDL_INIT_VIDEO);

	//Ask for an OpenGL context version 3.3, core profile, enable debug:
	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//create window:
	SDL_Window *window = SDL_CreateWindow(
		config.title.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		config.size.x, config.size.y,
		SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI*/
	);

	if (!window) {
		std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
		return 1;
	}

	//Create OpenGL context:
	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		SDL_DestroyWindow(window);
		std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
		return 1;
	}

	#ifdef _WIN32
	//On windows, load OpenGL extensions:
	if (!init_gl_shims()) {
		std::cerr << "ERROR: failed to initialize shims." << std::endl;
		return 1;
	}
	#endif

	//Set VSYNC + Late Swap (prevents crazy FPS):
	if (SDL_GL_SetSwapInterval(-1) != 0) {
		std::cerr << "NOTE: couldn't set vsync + late swap tearing (" << SDL_GetError() << ")." << std::endl;
		if (SDL_GL_SetSwapInterval(1) != 0) {
			std::cerr << "NOTE: couldn't set vsync (" << SDL_GetError() << ")." << std::endl;
		}
	}

	//Hide mouse cursor (note: showing can be useful for debugging):
	SDL_ShowCursor(SDL_DISABLE);

	//------------ opengl objects / game assets ------------

	//texture:
	GLuint tex = 0;
	glm::uvec2 tex_size = glm::uvec2(0,0);
	glm::uvec2 wolf_size = glm::uvec2(0,0);

	{ //load texture 'tex':
		std::vector< uint32_t > data;
		//load png files
		if (!load_png("elements.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load elements texture." << std::endl;
			exit(1);
		}
		if (!load_png("wolf.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load texture." << std::endl;
			exit(1);
		}
		if (!load_png("leopard.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load leopard texture." << std::endl;
			exit(1);
		}
		if (!load_png("lion.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load lion texture." << std::endl;
			exit(1);
		}		
		if (!load_png("player.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load player texture." << std::endl;
			exit(1);
		}
		if (!load_png("meat.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load meat texture." << std::endl;
			exit(1);
		}
		if (!load_png("tree.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load tree texture." << std::endl;
			exit(1);
		}
		if (!load_png("wizard.png", &tex_size.x, &tex_size.y, &data, LowerLeftOrigin)) {
			std::cerr << "Failed to load wizard texture." << std::endl;
			exit(1);
		}
		
		//create a texture object:
		glGenTextures(1, &tex);
		//bind texture object to GL_TEXTURE_2D:
		glBindTexture(GL_TEXTURE_2D, tex);
		//upload texture data from data:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		/*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_size.x, tex_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);*/
		//set texture sampling parameters:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	//shader program:
	GLuint program = 0;
	GLuint program_Position = 0;
	GLuint program_TexCoord = 0;
	GLuint program_Color = 0;
	GLuint program_mvp = 0;
	GLuint program_tex = 0;
	{ //compile shader program:
		GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER,
			"#version 330\n"
			"uniform mat4 mvp;\n"
			"in vec4 Position;\n"
			"in vec2 TexCoord;\n"
			"in vec4 Color;\n"
			"out vec2 texCoord;\n"
			"out vec4 color;\n"
			"void main() {\n"
			"	gl_Position = mvp * Position;\n"
			"	color = Color;\n"
			"	texCoord = TexCoord;\n"
			"}\n"
		);

		GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER,
			"#version 330\n"
			"uniform sampler2D tex;\n"
			"in vec4 color;\n"
			"in vec2 texCoord;\n"
			"out vec4 fragColor;\n"
			"void main() {\n"
			"	fragColor = texture(tex, texCoord) * color;\n"
			"}\n"
		);

		program = link_program(fragment_shader, vertex_shader);

		//look up attribute locations:
		program_Position = glGetAttribLocation(program, "Position");
		if (program_Position == -1U) throw std::runtime_error("no attribute named Position");
		program_TexCoord = glGetAttribLocation(program, "TexCoord");
		if (program_TexCoord == -1U) throw std::runtime_error("no attribute named TexCoord");
		program_Color = glGetAttribLocation(program, "Color");
		if (program_Color == -1U) throw std::runtime_error("no attribute named Color");

		//look up uniform locations:
		program_mvp = glGetUniformLocation(program, "mvp");
		if (program_mvp == -1U) throw std::runtime_error("no uniform named mvp");
		program_tex = glGetUniformLocation(program, "tex");
		if (program_tex == -1U) throw std::runtime_error("no uniform named tex");
	}

	//vertex buffer:
	GLuint buffer = 0;
	{ //create vertex buffer
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
	}

	struct Vertex {
		Vertex(glm::vec2 const &Position_, glm::vec2 const &TexCoord_, glm::u8vec4 const &Color_) :
			Position(Position_), TexCoord(TexCoord_), Color(Color_) { }
		glm::vec2 Position;
		glm::vec2 TexCoord;
		glm::u8vec4 Color;
	};
	static_assert(sizeof(Vertex) == 20, "Vertex is nicely packed.");

	//vertex array object:
	GLuint vao = 0;
	{ //create vao and set up binding:
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glVertexAttribPointer(program_Position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLbyte *)0);
		glVertexAttribPointer(program_TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLbyte *)0 + sizeof(glm::vec2));
		glVertexAttribPointer(program_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLbyte *)0 + sizeof(glm::vec2) + sizeof(glm::vec2));
		glEnableVertexAttribArray(program_Position);
		glEnableVertexAttribArray(program_TexCoord);
		glEnableVertexAttribArray(program_Color);
	}

	//------------ sprite info ------------
	struct SpriteInfo {
		glm::vec2 min_uv = glm::vec2(0.0f);
		glm::vec2 max_uv = glm::vec2(1.0f);
		glm::vec2 rad = glm::vec2(0.5f);
	};

	auto load_sprite = [](std::string const &name) -> SpriteInfo {
		SpriteInfo info;
		//TODO: look up sprite name in table of sprite infos
		return info;
	};

	auto random_float = [](float a, float b) {
		float random = ((float) rand()) / (float) RAND_MAX;
		float r = random * (b - a);
		return (a + r);
	};

	auto set_random_pos = [](glm::vec2 *objpos) {
		float random_x = ((float) rand()) / (float) RAND_MAX;
		float random_y = ((float) rand()) / (float) RAND_MAX;
		float r_x = (random_x * (2.0f) - 1.0f);
		float r_y = (random_y * (2.0f) - 1.0f);
		objpos->x = r_x;
		objpos->y = r_y;
		return objpos;
	};


	//------------ game state ------------

	//Initialization

	//Mouse
	glm::vec2 mouse = glm::vec2(0.0f, 0.0f); //mouse position in [-1,1]x[-1,1] coordinates

	//set positions of all living things
	glm::vec2 playerpos = glm::vec2(0.0f, 0.0f);
	glm::vec2 wolfpos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
	glm::vec2 lionpos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
	glm::vec2 leopos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
	glm::vec2 wizardpos = glm::vec2(1.6f, 0.2f); //off screen, but exists initially

	//place treeses
	struct tree {
		glm::vec2 position;
		float height;
		tree(glm::vec2 position, float height) : position(position), height(height) {}
	};

	std::vector<tree> treeScreen1;
	std::vector<tree> treeScreen2;
	std::vector<tree> treeScreen3;

	//place trees randomly throughout world
	for (int i = 0; i < 8; i++) {
		treeScreen1.emplace_back(glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f)), 1.0f);
		treeScreen2.emplace_back(glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f)), 1.0f);
		treeScreen3.emplace_back(glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f)), 1.0f);
	}

	//player bound variables
	float playerHealth = 1.0f;
	float playerTemp = 1.0f;
	//damage applied every frame from climate
	float healthDecay = 0.00037f;
	float tempDecay = 0.000037f;
	//inventory
	int lumber = 0;
	int meat = 0;
	//set amounts of trees
	int numTreesperScreen = 8;
	//amount of health meat replaces
	float meatRegen = 0.1f;
	//replenish 75% of health when wizard is offered meat
	float wizardRegen = 0.75f;

	//boolean triggers
	bool wolfCollide = false;
	bool lionCollide = false;
	bool leoCollide = false;
	bool wizardCollide = false;
	bool treeCollide = false;
	int treeCollideInstance = -1;

	//set initial speeds of interactable things
	float playerSpeed = 0.05f;
	float wolfSpeed = 0.0002f;
	float lionSpeed = 0.0003f;
	float leoSpeed = 0.0005f;
	float treeGrowRate = 0.0002f;

	//set amounts of meat animals drop
	int wolfMeat = 1;
	int leopardMeat = 2;
	int lionMeat = 3;

	//set amount of damage each animal causes per frame on contact
	float wolfDamage = 0.0002f;
	float leoDamage = 0.0003f;
	float lionDamage = 0.0005f;

	//animal livelyhood states
	bool wolfIsAlive = true;
	bool leoIsAlive = true;
	bool lionIsAlive = true;

	//set aggravated animal radius boxes
	float boxSizeMultiplier = 1000.0f;
	float wolfBox = wolfSpeed * boxSizeMultiplier;
	float lionBox = lionSpeed * boxSizeMultiplier;
	float leoBox = leoSpeed * boxSizeMultiplier;
	float wizardBox = 0.1f;
	float treeBox = 0.05f;

	//set timer to respawn
	float wolfDeadTime = 0.0f;
	float leoDeadTime = 0.0f;
	float lionDeadTime = 0.0f;

	//set respawn times
	float wolfSpawnTime = 30.0f; //wolf spawns every 30 seconds
	float leoSpawnTime = 60.0f; //leopard spawns every minute
	float lionSpawnTime = 120.0f; //lion spawns every other minutes
	float treeSpawnTime = 120.0f; //2 minutes to respawn tree

	//timer tracking total time of current game session
	float totalTime = 0.0f;

	//floats represent bool of that screens appearance
	//begin on middle screen
	glm::vec3 screen = glm::vec3(0.0f, 1.0f, 0.0f);

	struct {
		glm::vec2 at = glm::vec2(0.0f, 0.0f);
		glm::vec2 radius = glm::vec2(10.0f, 10.0f);
	} camera;
	//correct radius for aspect ratio:
	camera.radius.x = camera.radius.y * (float(config.size.x) / float(config.size.y));

	//------------ game loop ------------

	bool should_quit = false;
	while (true) {
		static SDL_Event evt;
		while (SDL_PollEvent(&evt) == 1) {
			//handle input:
			if (evt.type == SDL_MOUSEMOTION) {
				mouse.x = (evt.motion.x + 0.5f) / float(config.size.x) * 2.0f - 1.0f;
				mouse.y = (evt.motion.y + 0.5f) / float(config.size.y) *-2.0f + 1.0f;
			} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
			} else if (evt.type == SDL_KEYDOWN) {
				if (evt.key.keysym.sym == SDLK_ESCAPE)
					should_quit = true;

				//for walking
				else if (evt.key.keysym.sym == SDLK_w) {
					if (playerpos.y <= 1.0f)
						playerpos.y += playerSpeed;
				}
				else if (evt.key.keysym.sym == SDLK_s) {
					//check for lower boundaries
					if (playerpos.y >= -1.0f)
						playerpos.y -= playerSpeed;
				}
				else if (evt.key.keysym.sym == SDLK_d) {
					//check for right boundaries
					if (screen.z != 1.0f) {
						//currently not on leftmost screen
						if (playerpos.x >= 1.0f) {
							//place player into next screen
							playerpos.x = -std::abs(playerpos.x);
							if (screen.x == 1.0f) {
								screen.x = 0.0f;
								screen.y = 1.0f;
								//set random positions to animals since screens are separated
								if (wolfIsAlive) set_random_pos(&wolfpos);
								if (leoIsAlive) set_random_pos(&leopos);
								if (lionIsAlive) set_random_pos(&lionpos);
								wizardpos -= glm::vec2 (1.0f, 0.0f);
							}
							else if (screen.y == 1.0f) {
								screen.y = 0.0f;
								screen.z = 1.0f;
								if (wolfIsAlive) set_random_pos(&wolfpos);
								if (leoIsAlive) set_random_pos(&leopos);
								if (lionIsAlive) set_random_pos(&lionpos);
								wizardpos -= glm::vec2 (1.0f, 0.0f);
							}
						}
						else
							playerpos.x += playerSpeed;
					}
					else {
						if (playerpos.x < 1.0f)
							playerpos.x += playerSpeed;
					}
				}
				else if (evt.key.keysym.sym == SDLK_a) {
					//check for left boundaries
					if (screen.x != 1.0f) {
						//currently not on leftmost screen
						if (playerpos.x <= -1.0f) {
							//place player into next screen
							playerpos.x = std::abs(playerpos.x);
							if (screen.y == 1.0f) {
								screen.y = 0.0f;
								screen.x = 1.0f;
								if (wolfIsAlive) set_random_pos(&wolfpos);
								if (leoIsAlive) set_random_pos(&leopos);
								if (lionIsAlive) set_random_pos(&lionpos);
								wizardpos += glm::vec2 (1.0f, 0.0f);
							}
							else if (screen.z == 1.0f) {
								screen.z = 0.0f;
								screen.y = 1.0f;
								if (wolfIsAlive) set_random_pos(&wolfpos);
								if (leoIsAlive) set_random_pos(&leopos);
								if (lionIsAlive) set_random_pos(&lionpos);
								wizardpos += glm::vec2(1.0f, 0.0f);
							}
						}
						else
							playerpos.x -= playerSpeed;
					}
					else {
						if (playerpos.x > -1.0f)
							playerpos.x -= playerSpeed;
					}
				}

				//for interaction
				else if (evt.key.keysym.sym == SDLK_x) {
					//chop down tree or attack
					//animals
					if (wolfCollide) {
						wolfCollide = false;
						wolfSpeed = 0.0f;
						wolfpos = glm::vec2(10.0f, 10.0f);
					}
					else if (leoCollide) {
						leoCollide = false;
						leoSpeed = 0.0f;
						leopos = glm::vec2(10.0f, 10.0f);
					}
					else if (lionCollide) {
						lionCollide = false;
						lionSpeed = 0.0f;
						lionpos = glm::vec2(10.0f, 10.0f);
					}
					//tree
					if ((treeCollide) && (treeCollideInstance >= 0)) {
						if (screen.x == 1.0) {
							treeScreen1[treeCollideInstance].height = 0.0f;
						}
						else if (screen.y == 1.0) {
							treeScreen2[treeCollideInstance].height = 0.0f;
						}
						else
							treeScreen3[treeCollideInstance].height = 0.0f;

					}


				}
				else if (evt.key.keysym.sym == SDLK_f) {
					if (lumber > 0)
						lumber --;
				}
				else if (evt.key.keysym.sym == SDLK_e) {
					if (meat > 0) {
						meat --;
						if ((playerHealth + meatRegen) > 1.0f)
							playerHealth = 1.0f;
						else
							playerHealth += meatRegen;
					}
				}
				else if (evt.key.keysym.sym == SDLK_c) {
					if (wolfCollide)
						meat += wolfMeat;
					else if (leoCollide)
						meat += leopardMeat;
					else if (lionCollide)
						meat += lionMeat;
					//Player interacting with wizard
					else if (wizardCollide) {

						if (meat > 0) {
							meat --;
							if ((playerHealth + wizardRegen) > 1.0f)
								playerHealth = 1.0f;
							else
								playerHealth += wizardRegen;
						}
					    if (lumber > 0) {
					    	lumber --;
					    	if ((playerHealth + wizardRegen) > 1.0f)
					    		playerHealth = 1.0f;
					    	else
					    		playerHealth += wizardRegen;
					    }
					}
				}
			}
			else if (evt.type == SDL_QUIT) {
				should_quit = true;
				break;
			}
		}
		if (should_quit) break;

		auto current_time = std::chrono::high_resolution_clock::now();
		static auto previous_time = current_time;
		float elapsed = std::chrono::duration< float >(current_time - previous_time).count();
		previous_time = current_time;
		totalTime += elapsed;

		{ //update game state:
			(void)elapsed;
		}

		//draw output:
		glClearColor(0.5, 0.5, 0.5, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		{ //draw game state:
			std::vector< Vertex > verts;

			//helper: add rectangle to verts:
			auto rect = [&verts](glm::vec2 const &at, glm::vec2 const &rad, glm::u8vec4 const &tint) {
				verts.emplace_back(at + glm::vec2(-rad.x,-rad.y), glm::vec2(0.0f, 0.0f), tint);
				verts.emplace_back(verts.back());
				verts.emplace_back(at + glm::vec2(-rad.x, rad.y), glm::vec2(0.0f, 1.0f), tint);
				verts.emplace_back(at + glm::vec2( rad.x,-rad.y), glm::vec2(1.0f, 0.0f), tint);
				verts.emplace_back(at + glm::vec2( rad.x, rad.y), glm::vec2(1.0f, 1.0f), tint);
				verts.emplace_back(verts.back());
			};

			auto draw_sprite = [&verts](SpriteInfo const &sprite, glm::vec2 const &at) {
				glm::vec2 min_uv = sprite.min_uv;
				glm::vec2 max_uv = sprite.max_uv;
				glm::vec2 rad = sprite.rad;
				glm::u8vec4 tint = glm::u8vec4(0xff, 0xff, 0xff, 0xff);

				verts.emplace_back(at + glm::vec2(-rad.x,-rad.y), glm::vec2(min_uv.x, min_uv.y), tint);
				verts.emplace_back(verts.back());
				verts.emplace_back(at + glm::vec2(-rad.x, rad.y), glm::vec2(min_uv.x, max_uv.y), tint);
				verts.emplace_back(at + glm::vec2( rad.x,-rad.y), glm::vec2(max_uv.x, min_uv.y), tint);
				verts.emplace_back(at + glm::vec2( rad.x, rad.y), glm::vec2(max_uv.x, max_uv.y), tint);
				verts.emplace_back(verts.back());
			};

			//if the player is close enough to an animal, the animal will run towards player
			auto collision = [&playerpos](glm::vec2 *spritepos, float spriteRad, float spriteSpeed, bool *collision) {
				if ((playerpos.x > (spritepos->x - spriteRad)) && 
					(playerpos.x < (spritepos->x + spriteRad)) &&
					(playerpos.y > (spritepos->y - spriteRad)) &&
					(playerpos.y < (spritepos->y + spriteRad))) {
					//find vector to player pos, update position
					//spritepos->y = spritepos->y + spriteSpeed;
					if ((playerpos.x - spritepos->x) < 0)
						spritepos->x -= spriteSpeed;
					else if ((playerpos.x - spritepos->x) > 0)
						spritepos->x += spriteSpeed;
					if ((playerpos.y - spritepos->y) < 0)
						spritepos->y -= spriteSpeed;
					else if ((playerpos.y - spritepos->y) > 0)
						spritepos->y += spriteSpeed;
					*collision = true;
				}
				else
					collision = false;
			};

			//if animal reaches player, cause damage every frame
			auto direct_collision = [&playerpos, &playerHealth](glm::vec2 *spritepos, float spriteRad, float spriteDamage) {
				if ((playerpos.x > (spritepos->x - spriteRad)) && 
					(playerpos.x < (spritepos->x + spriteRad)) &&
					(playerpos.y > (spritepos->y - spriteRad)) &&
					(playerpos.y < (spritepos->y + spriteRad))) {
					playerHealth -= spriteDamage;
				}
			};

			//Constant decay on player
			playerHealth -= healthDecay;
			playerTemp -= tempDecay;

			if (playerHealth <= 0.0f)
				playerSpeed = 0.0f; //can't move if you're dead

			//respawn animals if respawn timer is up
			if (!wolfIsAlive && ((totalTime - wolfDeadTime) > wolfSpawnTime)) {
				wolfIsAlive = true;;
				wolfSpeed  = 0.0002f * (totalTime / 100.0f);
				wolfpos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				//each time an animal respawns, collision detection boxes on all animals will increase
				boxSizeMultiplier *= 1.5f;
			}
			if (!leoIsAlive && ((totalTime - leoDeadTime) > leoSpawnTime)) {
				leoIsAlive = true;
				leoSpeed = 0.0003f * (totalTime / 100.0f);
				leopos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				boxSizeMultiplier *= 1.5f;
			}
			if (!lionIsAlive && ((totalTime - lionDeadTime) > lionSpawnTime)) {
				lionIsAlive = true;
				lionSpeed = 0.0005f * (totalTime / 100.0f);
				lionpos = glm::vec2(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
				boxSizeMultiplier *= 1.5f;
			}

			//draw appropriate background
			rect(glm::vec2(-10.0f, 10.0f), glm::vec2(20.0f), glm::u8vec4(0xff, 0xff, 0xff, 0xff));

			//Draw a sprite "player" at position (5.0, 2.0):
			static SpriteInfo player = load_sprite("player");
			draw_sprite(player, playerpos * camera.radius + camera.at);
			static SpriteInfo wolf = load_sprite("wolf");
			draw_sprite(wolf, wolfpos * camera.radius + camera.at);
			static SpriteInfo leopard = load_sprite("leopard");
			draw_sprite(leopard, leopos * camera.radius + camera.at);
			static SpriteInfo lion = load_sprite("lion");
			draw_sprite(lion, lionpos * camera.radius + camera.at);
			static SpriteInfo wizard = load_sprite("wizard");
			draw_sprite(wizard, wizardpos * camera.radius + camera.at);
			static SpriteInfo tree = load_sprite("tree");
			static SpriteInfo stump = load_sprite("stump");
			if (screen.x == 1.0f) {
				for (int i = 0; i < numTreesperScreen; i ++) {
					if (treeScreen1[i].height < 1.0f)  {
						draw_sprite(stump, treeScreen1[i].position * camera.radius + camera.at);
						if ((treeScreen1[i].height + treeGrowRate) > 1.0f)
							treeScreen1[i].height = 1.0f;
						else
							treeScreen1[i].height += treeGrowRate;
					}
					else {
						draw_sprite(tree, treeScreen1[i].position * camera.radius + camera.at);
						collision(&(treeScreen1[i].position * camera.radius + camera.at), treeBox, 0.0f, &treeCollide);
					}
				}
			}
			else if (screen.y == 1.0f) {
				for (int i = 0; i < 8; i ++) {
					if (treeScreen2[i].height == 0.0f)
						draw_sprite(stump, treeScreen2[i].position * camera.radius + camera.at);
					else
						draw_sprite(tree, treeScreen2[i].position * camera.radius + camera.at);
				}
			}
			else if (screen.z == 1.0f) {
				for (int i = 0; i < 8; i ++) {
					if (treeScreen3[i].height == 0.0f)
						draw_sprite(stump, treeScreen3[i].position * camera.radius + camera.at);
					else
						draw_sprite(tree, treeScreen3[i].position * camera.radius + camera.at);
				}
			}

			collision(&wolfpos, wolfBox, wolfSpeed, &wolfCollide);
			collision(&leopos, leoBox, leoSpeed, &leoCollide);
			collision(&lionpos, lionBox, lionSpeed, &lionCollide);
			collision(&wizardpos, wizardBox, 0.0f, &wizardCollide);


			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * verts.size(), &verts[0], GL_STREAM_DRAW);

			glUseProgram(program);
			glUniform1i(program_tex, 0);
			glm::vec2 scale = 1.0f / camera.radius;
			glm::vec2 offset = scale * -camera.at;
			glm::mat4 mvp = glm::mat4(
				glm::vec4(scale.x, 0.0f, 0.0f, 0.0f),
				glm::vec4(0.0f, scale.y, 0.0f, 0.0f),
				glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
				glm::vec4(offset.x, offset.y, 0.0f, 1.0f)
			);
			glUniformMatrix4fv(program_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

			glBindTexture(GL_TEXTURE_2D, tex);
			glBindVertexArray(vao);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, verts.size());
		}


		SDL_GL_SwapWindow(window);
	}


	//------------  teardown ------------

	SDL_GL_DeleteContext(context);
	context = 0;

	SDL_DestroyWindow(window);
	window = NULL;

	return 0;
}



static GLuint compile_shader(GLenum type, std::string const &source) {
	GLuint shader = glCreateShader(type);
	GLchar const *str = source.c_str();
	GLint length = source.size();
	glShaderSource(shader, 1, &str, &length);
	glCompileShader(shader);
	GLint compile_status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE) {
		std::cerr << "Failed to compile shader." << std::endl;
		GLint info_log_length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetShaderInfoLog(shader, info_log.size(), &length, &info_log[0]);
		std::cerr << "Info log: " << std::string(info_log.begin(), info_log.begin() + length);
		glDeleteShader(shader);
		throw std::runtime_error("Failed to compile shader.");
	}
	return shader;
}

static GLuint link_program(GLuint fragment_shader, GLuint vertex_shader) {
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	GLint link_status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE) {
		std::cerr << "Failed to link shader program." << std::endl;
		GLint info_log_length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		std::vector< GLchar > info_log(info_log_length, 0);
		GLsizei length = 0;
		glGetProgramInfoLog(program, info_log.size(), &length, &info_log[0]);
		std::cerr << "Info log: " << std::string(info_log.begin(), info_log.begin() + length);
		throw std::runtime_error("Failed to link program");
	}
	return program;
}
