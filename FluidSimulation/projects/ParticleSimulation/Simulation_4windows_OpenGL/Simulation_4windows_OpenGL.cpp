#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <Eigen/SparseCore>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/Dense>
#include <chrono>
#include <iostream>
#include <Windows.h>

#include "../../../grid/grid.h"
#include "../../../grid/drawgrid.h"
#include "../../../Simulation/fluid_grid_2D.h"
#include "../../../box/box.h"
#include "../../../Simulation/Constant_Acceleration_Simulation_2D.h"
#include "../../../Simulation/gather_simulation.h"
#include "../../../Simulation/simul_sinecosine.h"
#include "MetaballRenderer.h"


using namespace std;

//window size
int Width = 1024;
int Height = 1024;

GLuint model_view; // model-view matrix uniform shader variable location

//Projection transformation parameters
GLuint projection; //projection matrix uniform shader variable location

GLuint vao;
GLuint vbo;

//time
int time_idle;

//simulation ����
unique_ptr<Fluid_Simulator_Grid> simulation;
unique_ptr<Constant_Acceleration_Simulator> constant_acceleration_simulation;
unique_ptr<Simul_SineCosine> sinecosine_simulation;
unique_ptr<GatherSimulation> gather_simulation;

//n���� - �̻��
int n = 12;

//number�� �����ϸ� particle ���� �ٲ��.
int number = 4000;

unique_ptr<vector<Vector2D>> fluids_points;
unique_ptr<vector<Vector2D>> constant_acceleration_points;
unique_ptr<vector<Vector2D>> cosine_points;
unique_ptr<vector<Vector2D>> sine_points;
unique_ptr<vector<Vector2D>> gather_points;
unique_ptr<vector<vec3>> color;

Box bbox;
vector<Vector2D> grid_line;
vector<Vector2D> box_line;

//grid_N�� �����ϸ� grid ���� �ٲ��.
int grid_N = 40;

//�ùķ��̼� ���¸� ������ option
int Option = 3;

bool isStart = false;
bool isPixelMode = false;
bool isParticleMode = true;
bool isMetaballMode = false;
//bool isExtrapolationCell = false;

// 색상 초기화 상태를 추적하는 플래그
bool colors_initialized = false;

// Metaball renderer
unique_ptr<MetaballRenderer> metaballRenderer;
GLuint metaballVAO, metaballVBO, metaballEBO;

//========================================================particle mode ========================================================//

//simulation�� particle���� ��ġ�� points�� ����
void pushback_SimulationPoints_to_Points() {
	fluids_points->clear();
	for (int i = 0; i < number; i++) {
		fluids_points->push_back(simulation->particles[i].Location);
	}
}

void pushback_constant_acceleration_SimulationPoints_to_Points() {
	constant_acceleration_points->clear();
	for (int i = 0; i < number; i++) {
		constant_acceleration_points->push_back(constant_acceleration_simulation->particles[i].Location);
	}
}

void pushback_sinecosine_SimulationPoints_to_Points() {
	cosine_points->clear();
	sine_points->clear();
	for (int i = 0; i < number; i++) {
		cosine_points->push_back((*sinecosine_simulation->cosine_particles)[i]);
		sine_points->push_back((*sinecosine_simulation->sine_particles)[i]);
	}
}

void pushback_gather_SimulationPoints_to_Points() {
	gather_points->clear();
	for (int i = 0; i < number; i++) {
		gather_points->push_back(gather_simulation->particles[i].Location);
	}
}

//========================================================metaball mode ========================================================//

void initMetaballRenderer() {
	// Initialize metaball renderer with simulation bounds
	Vector2D minBounds(-1.0f, -1.0f);
	Vector2D maxBounds(1.0f, 1.0f);
	metaballRenderer = make_unique<MetaballRenderer>(64, 64, minBounds, maxBounds);
	
	// Set metaball parameters
	metaballRenderer->setThreshold(0.5f);
	metaballRenderer->setRadius(0.15f);
	metaballRenderer->setStrength(1.5f);
	
	// Generate OpenGL buffers for metaball mesh
	glGenVertexArrays(1, &metaballVAO);
	glGenBuffers(1, &metaballVBO);
	glGenBuffers(1, &metaballEBO);
}

void generateMetaballMesh() {
	if (!metaballRenderer) return;
	
	// Convert fluid particles to Vector2D format for metaball renderer
	std::vector<Vector2D> particles;
	for (int i = 0; i < number; i++) {
		particles.push_back(simulation->particles[i].Location);
	}
	
	// Generate scalar field and mesh
	metaballRenderer->generateScalarField(particles);
	metaballRenderer->generateMesh();
}

void updateMetaballBuffers() {
	if (!metaballRenderer) return;
	
	const MetaballMesh& mesh = metaballRenderer->getMesh();
	
	if (mesh.vertices.empty()) return;
	
	// Bind VAO
	glBindVertexArray(metaballVAO);
	
	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, metaballVBO);
	glBufferData(GL_ARRAY_BUFFER, 
				 mesh.vertices.size() * sizeof(Vector2D), 
				 mesh.vertices.data(), 
				 GL_DYNAMIC_DRAW);
	
	// Update element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, metaballEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				 mesh.indices.size() * sizeof(unsigned int), 
				 mesh.indices.data(), 
				 GL_DYNAMIC_DRAW);
	
	// Set vertex attributes
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2D), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);
}

void renderMetaballMesh() {
	if (!metaballRenderer) return;
	
	const MetaballMesh& mesh = metaballRenderer->getMesh();
	
	if (mesh.indices.empty()) return;
	
	// Set metaball color (light blue for fluid surface)
	glColor3f(0.3f, 0.7f, 1.0f);
	
	// Enable line smoothing for better visual quality
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	// Render as wireframe to show the surface outline
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	
	glBindVertexArray(metaballVAO);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	// Reset polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.0f);
	glDisable(GL_LINE_SMOOTH);
}

// Global flag to track if particle data has changed
bool particle_data_changed = true;

void pushback_color() {
    // 이미 초기화되었고 파티클 데이터가 변경되지 않았다면 리턴
    if (colors_initialized && !particle_data_changed) {
        return;
    }

    // 처음 초기화할 때만 벡터 크기를 계산
    if (!colors_initialized) {
        size_t total_size = 
            number +  // Fluid particles
            number +  // Constant acceleration particles
            (4 + 4 * (grid_N - 1)) +  // Grid lines
            simulation->fluid_cell_center_point->size() +  // Fluid cell centers
            2 * sinecosine_simulation->particle_num +  // Sine and cosine particles
            number;  // Gather particles

        color->resize(total_size);
        
        // Debug: Log initial setup
        std::cout << "Debug: Initial color setup, total size: " << total_size << std::endl;
    } else {
        // Debug: Log update reason
        std::cout << "Debug: Skipping color update - colors are fixed" << std::endl;
        particle_data_changed = false;
        return;
    }

    size_t current_index = 0;

    // Fluid particles - blue
    for (int i = 0; i < number; i++) {
        (*color)[current_index++] = vec3(0.0f, 0.0f, 1.0f);
    }

    // Constant acceleration particles - green
    for (int i = 0; i < number; i++) {
        (*color)[current_index++] = vec3(0.0f, 1.0f, 0.0f);
    }

    // Box and Grid lines - black (boundary)
    size_t boundary_start = current_index;
    for (int i = 0; i < 4 + 4 * (grid_N - 1); i++) {
        (*color)[current_index++] = vec3(0.0f, 0.0f, 0.0f);
    }

    // Store boundary color indices for later use
    static size_t boundary_color_start = boundary_start;
    static size_t boundary_color_count = current_index - boundary_start;

    // Fluid mode cell centers - cyan
    for (size_t i = 0; i < simulation->fluid_cell_center_point->size(); i++) {
        (*color)[current_index++] = vec3(0.0f, 1.0f, 1.0f);
    }

    // Sine and cosine particles - red and yellow
    for (int i = 0; i < sinecosine_simulation->particle_num; i++) {
        (*color)[current_index++] = vec3(1.0f, 0.0f, 0.0f); // Sine particles
        (*color)[current_index++] = vec3(1.0f, 1.0f, 0.0f); // Cosine particles
    }

    // Gather particles - magenta
    for (int i = 0; i < number; i++) {
        (*color)[current_index++] = vec3(1.0f, 0.0f, 1.0f);
    }

    // Set color initialization flag only once at the end
    colors_initialized = true;
    particle_data_changed = false;
}

//�ùķ��̼� ����� ���� ���ڵ��� ���� Update��.
void Update_Points() {
	bool changed = false;
	for (int i = 0; i < simulation->particles.size(); i++) {
		Vector2D newLoc = simulation->particles[i].Location;
		if ((*fluids_points)[i] != newLoc) {
			(*fluids_points)[i] = newLoc;
			changed = true;
		}
	}
	if (changed) particle_data_changed = true;
}

void Update_constant_Points() {
    bool changed = false;
    for (int i = 0; i < constant_acceleration_simulation->particles.size(); i++) {
        Vector2D newLoc = constant_acceleration_simulation->particles[i].Location;
        if ((*constant_acceleration_points)[i] != newLoc) {
            (*constant_acceleration_points)[i] = newLoc;
            changed = true;
        }
    }
    if (changed) particle_data_changed = true;
}

void Update_sinecosine_Points() {
    bool changed = false;
    for (int i = 0; i < sinecosine_simulation->particle_num; i++) {
        Vector2D newCosLoc = (*sinecosine_simulation->cosine_particles)[i];
        Vector2D newSinLoc = (*sinecosine_simulation->sine_particles)[i];
        
        if ((*cosine_points)[i] != newCosLoc || (*sine_points)[i] != newSinLoc) {
            (*cosine_points)[i] = newCosLoc;
            (*sine_points)[i] = newSinLoc;
            changed = true;
        }
    }
    if (changed) particle_data_changed = true;
}

void Update_gather_Points() {
    bool changed = false;
    for (int i = 0; i < gather_simulation->particles.size(); i++) {
        Vector2D newLoc = gather_simulation->particles[i].Location;
        if ((*gather_points)[i] != newLoc) {
            (*gather_points)[i] = newLoc;
            changed = true;
        }
    }
    if (changed) particle_data_changed = true;
}

//========================================================particle mode ========================================================//



//========================================================circle mode========================================================//
//�� n������ ��ǥ ���� - �̻��

void pushback_Circle_points() {
	Vector2D location;
	fluids_points->clear();
	//j��° particle
	for (int j = 0; j < number; j++) {
		location = simulation->particles[j].Location;
		for (int i = 0; i < n; i++) {
			int theta = 360 / n;
			double x = location.X + 0 * cos(theta * i);
			double y = location.Y + 0 * sin(theta * i);

			Vector2D new_location = Vector2D(x, y);

			fluids_points->push_back(new_location);
		}
	}
}

void Update_Circle_points() {
	Vector2D location;
	//j��° particle
	for (int i = 0; i < number; i++) {
		location = simulation->particles[i].Location;
		for (int j = 0; j < n; j++) {
			int theta = 360 / n;
			double x = location.X + 0 * cos(theta * j);
			double y = location.Y + 0 * sin(theta * j);

			(*fluids_points)[i * n + j] = Vector2D(x, y);
		}
	}
}

void pushback_Circle_color() {
	color->clear();

	for (int i = 0; i < n * number; i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	for (int i = 0; i < 4 + 4 * (grid_N - 1); i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//fluid mode�� blue
	for (int i = 0; i < simulation->fluid_cell_center_point->size(); i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}
}

//========================================================circle mode========================================================//

void init(void) {

	fluids_points = make_unique<vector<Vector2D>>();
	constant_acceleration_points = make_unique<vector<Vector2D>>();
	cosine_points = make_unique<vector<Vector2D>>();
	sine_points = make_unique<vector<Vector2D>>();
	gather_points = make_unique<vector<Vector2D>>();
	color = make_unique<vector<vec3>>();

	//simulation ���� �� ���ڵ� ����
	simulation = make_unique<Fluid_Simulator_Grid>(number, grid_N);
	constant_acceleration_simulation = make_unique<Constant_Acceleration_Simulator>(number, grid_N);
	sinecosine_simulation = make_unique<Simul_SineCosine>(number);
	gather_simulation = make_unique<GatherSimulation>(number);

	//fluid
	pushback_SimulationPoints_to_Points();
	//pushback_Circle_points();

	//constant
	pushback_constant_acceleration_SimulationPoints_to_Points();

	//sinecosine
	pushback_sinecosine_SimulationPoints_to_Points();

	//gather
	pushback_gather_SimulationPoints_to_Points();

	pushback_color();
	//pushback_Circle_color();

	// Initialize metaball renderer
	initMetaballRenderer();

		// bbox 초기화 (한 번만 설정되는 정적 경계)
	bbox = Box(0.0, 1.0, 0.0, 1.0);
	box_line = { 
		Vector2D(bbox.xmin, bbox.ymin),
		Vector2D(bbox.xmax, bbox.ymin),
		Vector2D(bbox.xmax, bbox.ymax),
		Vector2D(bbox.xmin, bbox.ymax)
	};

	// grid 선 초기화 (한 번만 설정되는 정적 경계)
	grid_line.reserve(4 * (grid_N - 1));  // 미리 메모리 할당
	for (int i = 0; i < 4 * (grid_N - 1); i++) {
		grid_line.push_back(Vector2D(0, 0));
	}

	// grid 점 생성 (한 번만 실행)
	make_Points_of_grids(grid_line, grid_N);

	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	glGenBuffers(1, &(vbo));
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * color->size(), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) * sine_points->size() + sizeof(Vector2D) * gather_points->size() + sizeof(Vector2D) * color->size(), NULL, GL_STATIC_DRAW);

	//particle�� ������
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * fluids_points->size(), &((*fluids_points)[0]));
	//box ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size(), sizeof(Vector2D) * box_line.size(), &(box_line[0]));
	//grid ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size(), sizeof(Vector2D) * grid_line.size(), &(grid_line[0]));

	//fluid cell center point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));

	////air cell center point ������
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * 
	//	simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));

	//constant acceleration point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * constant_acceleration_points->size(), &((*constant_acceleration_points)[0]));

	//sinecosine point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size(), sizeof(Vector2D) * cosine_points->size(), &((*cosine_points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size(), sizeof(Vector2D) * sine_points->size(), &((*sine_points)[0]));

	//gather point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) *
		sine_points->size(), sizeof(Vector2D) * gather_points->size(), &((*gather_points)[0]));

	//color 데이터는 particle_data_changed가 true일 때만 업데이트
    if (particle_data_changed) {
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
            * simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) * sine_points->size() + sizeof(Vector2D) * gather_points->size(), sizeof(vec3) * color->size(), &((*color)[0]));
    }

	//load shaders
	GLuint program = InitShader("vshader_2dBezier_test.glsl", "fshader_2dBezier_test.glsl");
	glUseProgram(program);

	//points memory position ( points and box )
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), BUFFER_OFFSET(0));

	//color position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size()));

	glEnableVertexAttribArray(0);
	//initialize uniform variable from vertex shander
	model_view = glGetUniformLocation(program, "model_view");
	projection = glGetUniformLocation(program, "projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 0.0);
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	//glViewport(width/2 , 0 , width/2, height / 2);

}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		exit(EXIT_SUCCESS);
		break;
	case 'm':
	case 'M':
		// Toggle metaball mode
		isMetaballMode = !isMetaballMode;
		if (isMetaballMode) {
			isParticleMode = false;
			isPixelMode = false;
			printf("Metaball mode enabled\n");
		} else {
			isParticleMode = true;
			printf("Particle mode enabled\n");
		}
		break;
	case 'p':
	case 'P':
		// Toggle particle mode
		isParticleMode = !isParticleMode;
		if (isParticleMode) {
			isPixelMode = false;
			isMetaballMode = false;
		}
		break;

	}
	glutPostRedisplay();
}

void idle(void)
{
	////����
	//printf("\n__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");

	//������ �и��� ���
	time_idle = glutGet(GLUT_ELAPSED_TIME);

	//0.06�� ���� particle ��ġ update
	if (time_idle % 120 == 0) {
		if (isStart) {
			//auto start = std::chrono::system_clock::now();

			//particle
			simulation->particle_simulation();

			/*std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
			std::cout << "simulation �ɸ��� �ð�(��) : " << sec.count() << "seconds" << std::endl;*/

			//constant
			constant_acceleration_simulation->particle_simulation();

			//sinecosine
			sinecosine_simulation->particle_simulation();

			//gather
			gather_simulation->particle_simulation();


			//=====================================================================================================

			/*std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
			std::cout << "simulation �ɸ��� �ð�(��) : " << sec.count() << "seconds" << std::endl;*/

            // 이전 위치를 저장
            particle_data_changed = false;
            
            // 각 시뮬레이션의 파티클 위치 업데이트
            Update_Points();
            Update_constant_Points();
            Update_sinecosine_Points();
            Update_gather_Points();

            // 실제 변경이 있을 때만 색상 업데이트
            if (particle_data_changed) {
                pushback_color();
            }

            // Update metaball mesh if in metaball mode
            if (isMetaballMode) {
                generateMetaballMesh();
                updateMetaballBuffers();
            }

            ////circle
            //simulation->particle_simulation();
            //Update_Circle_points();
		}
	}

	glutPostRedisplay();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, Width / 2, Height / 2);

	//ȭ�� mapping 
	mat4 p = Ortho2D(-1.0, 1.0, -1.0, 1.0); // ��� �׷��� ���
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	//변경된 좌표 다시 메모리에 넣기
	glBindVertexArray(vao);
	
	// 파티클 좌표 업데이트
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * fluids_points->size(), &((*fluids_points)[0]));

	// box와 grid는 정적이므로 업데이트하지 않음
	
	// 셀 중심점 업데이트
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * constant_acceleration_points->size(), &((*constant_acceleration_points)[0]));
	/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) *
		simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));*/
		//sinecosine point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size(), sizeof(Vector2D) * cosine_points->size(), &((*cosine_points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size(), sizeof(Vector2D) * sine_points->size(), &((*sine_points)[0]));
	//gather point ������
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) *
		sine_points->size(), sizeof(Vector2D) * gather_points->size(), &((*gather_points)[0]));


	glViewport(0, 0, Width / 2, Height / 2);

	//================================fluid �׸���
	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//particle
			glDrawArrays(GL_POINTS, 0, fluids_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
		
		// Render metaball surface if in metaball mode
		if (isMetaballMode) {
			renderMetaballMesh();
		}
	}

	//box �׸���
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);

	//grid �׸���
	//glDrawArrays(GL_LINES, points->size() + 4, 4 * (grid_N-1));

	glPointSize(10.0);
	if (isStart) {
		if (isPixelMode) {
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1), simulation->fluid_cell_center_point->size());
		}
		//if (isExtrapolationCell) {
		//	//glLineWidth(0.05);
		//	//glDrawArrays(GL_LINES, points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());

		//	glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
		//}
	}

	//================================constant �׸���
	glViewport(0, Height / 2, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//particle
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), constant_acceleration_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box �׸���
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);


	//================================sinecosine �׸���
	glViewport(Width / 2, 0, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//particle
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size(), cosine_points->size());
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size() + cosine_points->size(), sine_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box �׸���
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);


	//================================gather �׸���
	glViewport(Width / 2, Height / 2, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//particle
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size() +
				cosine_points->size() + sine_points->size(), gather_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box �׸���
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);

	//grid �׸���
	//glDrawArrays(GL_LINES, points->size() + 4, 4 * (grid_N-1));

	//glPointSize(10.0);
	//if (isStart) {
	//	if (isPixelMode) {
	//		glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1), simulation->fluid_cell_center_point->size());
	//	}
	//	if (isExtrapolationCell) {
	//		//glLineWidth(0.05);
	//		//glDrawArrays(GL_LINES, points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());

	//		glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
	//	}
	//}


	glBindVertexArray(0);
	glutSwapBuffers();
}

void Menu(int Option) {

	switch (Option) {
		//particlemode
	case 0:
		isParticleMode = true;
		isPixelMode = false;
		isMetaballMode = false;
		break;

		//fluidmode
	case 1:
		isParticleMode = false;
		isPixelMode = true;
		isMetaballMode = false;
		break;

		//metaballmode
	case 4:
		isParticleMode = false;
		isPixelMode = false;
		isMetaballMode = true;
		break;

		//start
	case 2:
		isStart = true;
		break;

		//finish
	case 3:
		isStart = false;
		simulation->clear_and_ReInit();
		break;

		//extra_visible
	/*case 4:
		isExtrapolationCell = !isExtrapolationCell;
		break;*/

	}

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(Width, Height);

	glutCreateWindow("2D Particle Simulation");

	glewInit();
	init();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutCreateMenu(Menu);
	glutAddMenuEntry("ParticleMode", 0);
	glutAddMenuEntry("PixelMode", 1);
	glutAddMenuEntry("Start", 2);
	glutAddMenuEntry("Finish", 3);
	glutAddMenuEntry("MetaballMode", 4);
	//glutAddMenuEntry("extra_visible", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}

// Debug function to log the contents of fluid_cell_center_point
void log_fluid_cell_center_point() {
    std::cout << "Debug: fluid_cell_center_point size: " << simulation->fluid_cell_center_point->size() << std::endl;
    for (size_t i = 0; i < simulation->fluid_cell_center_point->size(); ++i) {
        std::cout << "Point " << i << ": (" << (*simulation->fluid_cell_center_point)[i].X << ", "
                  << (*simulation->fluid_cell_center_point)[i].Y << ")" << std::endl;
    }
} 
