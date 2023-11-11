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
#include <thread>


#include "../header_background/grid.h"
#include "../header_background/drawgrid.h"
#include "../Simulation/fluid_grid_2D_multithread.h"
#include "../Simulation/Constant_Acceleration_Simulation_2D.h"
#include "../Simulation/simul_sinecosine.h"
#include "../Simulation/gather_simulation.h"
#include "../header_background/box.h"


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

unique_ptr<vector<Vector2D>> fluids_points;
unique_ptr<vector<Vector2D>> constant_acceleration_points;
unique_ptr<vector<Vector2D>> cosine_points;
unique_ptr<vector<Vector2D>> sine_points;
unique_ptr<vector<Vector2D>> gather_points;
unique_ptr<vector<vec3>> color;

unique_ptr<Fluid_Simulator_Grid> simulation;
unique_ptr<Constant_Acceleration_Simulator> constant_acceleration_simulation;
unique_ptr<Simul_SineCosine> sinecosine_simulation;
unique_ptr<GatherSimulation> gather_simulation;

//n각형
int n = 12;

//number를 조절하면 particle 수가 바뀐다.
int number = 4000;

Box bbox;
vector<Vector2D> grid_line;
vector<Vector2D> box_line;

//grid_N을 조절하면 grid 수가 바뀐다.
int grid_N = 40;

//시뮬레이션 상태를 조절할 option
int Option = 3;

bool isStart = false;
bool isPixelMode = false;
bool isParticleMode = true;
//bool isExtrapolationCell = false;

//============================================================================================================================

//========================================================particle mode ========================================================//

//simulation의 particle들의 위치를 points에 저장
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

void pushback_color() {
	color->clear();

	//fluid
	for (int i = 0; i < number; i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//constant
	for (int i = 0; i < number; i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	for (int i = 0; i < 4 + 4 * (grid_N - 1); i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//fluid mode는 blue
	for (int i = 0; i < simulation->fluid_cell_center_point->size(); i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//sinecosine
	for (int i = 0; i < sinecosine_simulation->particle_num; i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//gather
	for (int i = 0; i < number; i++) {
		color->push_back(vec3(0.0f, 0.0f, 0.0f));
	}

}

void Update_Points() {
	for (int i = 0; i < simulation->particles.size(); i++) {
		(*fluids_points)[i] = simulation->particles[i].Location;
	}
}

void Update_constant_Points() {
	for (int i = 0; i < constant_acceleration_simulation->particles.size(); i++) {
		(*constant_acceleration_points)[i] = constant_acceleration_simulation->particles[i].Location;
	}
}

void Update_sinecosine_Points() {
	for (int i = 0; i < sinecosine_simulation->particle_num; i++) {
		(*cosine_points)[i] = (*sinecosine_simulation->cosine_particles)[i];
		(*sine_points)[i] = (*sinecosine_simulation->sine_particles)[i];
	}
}

void Update_gather_Points() {
	for (int i = 0; i < gather_simulation->particles.size(); i++) {
		(*gather_points)[i] = gather_simulation->particles[i].Location;
	}
}

//========================================================particle mode ========================================================//



//========================================================circle mode========================================================//
//정 n각형의 좌표 생성

void pushback_Circle_points() {
	Vector2D location;
	fluids_points->clear();
	//j번째 particle
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
	//j번째 particle
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

	//fluid mode는 blue
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

	//simulation 실행 및 입자들 생성
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

	//bbox 선언
	bbox = Box(0.0, 1.0, 0.0, 1.0);
	box_line.push_back(Vector2D(bbox.xmin, bbox.ymin));
	box_line.push_back(Vector2D(bbox.xmax, bbox.ymin));
	box_line.push_back(Vector2D(bbox.xmax, bbox.ymax));
	box_line.push_back(Vector2D(bbox.xmin, bbox.ymax));

	//vector의 size를 원하는 크기만큼 늘린다.
	for (int i = 0; i < 4 * (grid_N - 1); i++) {
		grid_line.push_back(Vector2D(0, 0));
	}

	//grid의 점을 그리는 함수
	make_Points_of_grids(grid_line, grid_N);

	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	glGenBuffers(1, &(vbo));
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * color->size(), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) * sine_points->size() + sizeof(Vector2D) * gather_points->size() + sizeof(Vector2D) * color->size(), NULL, GL_STATIC_DRAW);

	//particle들 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * fluids_points->size(), &((*fluids_points)[0]));
	//box 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size(), sizeof(Vector2D) * box_line.size(), &(box_line[0]));
	//grid 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size(), sizeof(Vector2D) * grid_line.size(), &(grid_line[0]));

	//fluid cell center point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));

	////air cell center point 렌더링
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * 
	//	simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));

	//constant acceleration point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * constant_acceleration_points->size(), &((*constant_acceleration_points)[0]));

	//sinecosine point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size(), sizeof(Vector2D) * cosine_points->size(), &((*cosine_points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size(), sizeof(Vector2D) * sine_points->size(), &((*sine_points)[0]));

	//gather point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) *
		sine_points->size(), sizeof(Vector2D) * gather_points->size(), &((*gather_points)[0]));

	//color 할당
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) * sine_points->size() + sizeof(Vector2D) * gather_points->size(), sizeof(vec3) * color->size(), &((*color)[0]));

	//load shaders
	GLuint program = InitShader("simulation/2D/src/vshader_2dBezier_test.glsl", "simulation/2D/src/fshader_2dBezier_test.glsl");
	glUseProgram(program);

	//points memory position ( points and box )
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), BUFFER_OFFSET(0));

	//color position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), BUFFER_OFFSET(sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size()));

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

	}
	glutPostRedisplay();
}

void idle(void)
{
	////구분
	//printf("\n__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");
	//printf("__________________________________________________________________________________\n");

	//누적된 밀리초 얻기
	time_idle = glutGet(GLUT_ELAPSED_TIME);

	//0.06초 마다 particle 위치 update
	if (time_idle % 120 == 0) {
		if (isStart) {
			std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

			//particle


			//// fluid
			////simulation->particle_simulation();
			//fluid_simulation();


			////constant
			//thread t1{ &Constant_Acceleration_Simulator::Update_particles_Velocity_Thread, constant_acceleration_simulation, 0 , number / 2 };
			//thread t2{ &Constant_Acceleration_Simulator::Update_particles_Velocity_Thread, constant_acceleration_simulation, number/2 , number };
			//t1.join();
			//t2.join();

			//thread t3{ &Constant_Acceleration_Simulator::Update_particles_Location_Thread, constant_acceleration_simulation, 0 , number/2 };
			//thread t4{ &Constant_Acceleration_Simulator::Update_particles_Location_Thread, constant_acceleration_simulation, number / 2 , number };
			//t3.join();
			//t4.join();


			////sinecosine
			//thread t5{ &Simul_SineCosine::accumulation_time, sinecosine_simulation };
			//t5.join();


			//thread t6{ &Simul_SineCosine::calculate_sine_thread, sinecosine_simulation, 0 , number / 2 };
			//thread t7{ &Simul_SineCosine::calculate_sine_thread, sinecosine_simulation, number / 2 , number };
			//thread t8{ &Simul_SineCosine::calculate_cosine_thread, sinecosine_simulation, 0 , number / 2 };
			//thread t9{ &Simul_SineCosine::calculate_cosine_thread, sinecosine_simulation, number / 2 , number };
			//t6.join();
			//t7.join();
			//t8.join();
			//t9.join();


			////gather
			//thread t10 { &GatherSimulation::Update_Acceleration_thread, gather_simulation, 0 , number / 2 };
			//thread t11{ &GatherSimulation::Update_Acceleration_thread, gather_simulation, number/2 , number  };
			//t10.join();
			//t11.join();

			//thread t12{ &GatherSimulation::Update_Velocity_thread, gather_simulation, 0 , number / 2 };
			//thread t13{ &GatherSimulation::Update_Velocity_thread, gather_simulation, number / 2 , number };
			//t12.join();
			//t13.join();

			//thread t14{ &GatherSimulation::Update_Location_thread, gather_simulation, 0 , number / 2 };
			//thread t15{ &GatherSimulation::Update_Location_thread, gather_simulation, number / 2 , number };
			//t14.join();
			//t15.join();

			//과거의 멀티스레드 코드
			//==========================================================================================
			//오히려 단순하게 코딩한 이쪽이 컨텍스트 스위칭이 적어서 더 좋다..!
			//위의 코드는 메인 스레드가 너무 왔다갔다하는 단점이 있다.

			//메인스레드 + 다른 스레드의 시뮬레이션	
			simulation->particle_simulation();
			thread t2{ &Constant_Acceleration_Simulator::particle_simulation, constant_acceleration_simulation.get()};
			thread t3{ &Simul_SineCosine::particle_simulation, sinecosine_simulation.get()};
			thread t4{ &GatherSimulation::particle_simulation, gather_simulation.get() };

			t2.join();
			t3.join();
			t4.join();


			std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
			std::cout << "simulation 걸리는 시간(초) : " << sec.count() << "seconds" << std::endl;

			//시뮬레이션 결과에 따른 points update
			Update_Points();
			Update_constant_Points();
			Update_sinecosine_Points();
			Update_gather_Points();

			//pushback_color();

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

	//화면 mapping 
	mat4 p = Ortho2D(-1.0, 1.0, -1.0, 1.0); // 곡선이 그려질 평면
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	//바뀐 좌표 다시 메모리에 넣기
	glBindVertexArray(vao);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * fluids_points->size(), &((*fluids_points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * constant_acceleration_points->size(), &((*constant_acceleration_points)[0]));
	/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) *
		simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));*/
		//sinecosine point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size(), sizeof(Vector2D) * cosine_points->size(), &((*cosine_points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() +
		sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size(), sizeof(Vector2D) * sine_points->size(), &((*sine_points)[0]));
	//gather point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * fluids_points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D)
		* simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * constant_acceleration_points->size() + sizeof(Vector2D) * cosine_points->size() + sizeof(Vector2D) *
		sine_points->size(), sizeof(Vector2D) * gather_points->size(), &((*gather_points)[0]));

	//window의 4분의 1 할당
	glViewport(0, 0, Width / 2, Height / 2);

	//================================fluid 그리기
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
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);

	//grid 그리기
	//glDrawArrays(GL_LINES, points->size() + 4, 4 * (grid_N-1));

	glPointSize(10.0);
	if (isStart) {
		if (isPixelMode) {
			//시뮬레이션 결과를 메모리에 넣기
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1), simulation->fluid_cell_center_point->size());
		}
		//if (isExtrapolationCell) {
		//	//glLineWidth(0.05);
		//	//glDrawArrays(GL_LINES, points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());

		//	glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
		//}
	}

	//window의 4분의 1 할당
	glViewport(0, Height / 2, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//시뮬레이션 결과를 메모리에 넣기
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), constant_acceleration_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);


	//================================sinecosine 그리기
	glViewport(Width / 2, 0, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//시뮬레이션 결과를 메모리에 넣기
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size(), cosine_points->size());
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size() + cosine_points->size(), sine_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);


	//window의 4분의 1 할당
	glViewport(Width / 2, Height / 2, Width / 2, Height / 2);

	glPointSize(2.0);
	glLineWidth(1.0);

	if (isStart) {
		if (isParticleMode) {
			//시뮬레이션 결과를 메모리에 넣기
			glDrawArrays(GL_POINTS, fluids_points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size() + constant_acceleration_points->size() +
				cosine_points->size() + sine_points->size(), gather_points->size());

			////circle
			//for (int i = 0; i < number; i++) {
			//	glDrawArrays(GL_LINES, i * n, n );
			//}
		}
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, fluids_points->size(), 4);

	//grid 그리기
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
		break;

		//fluidmode
	case 1:
		isParticleMode = false;
		isPixelMode = true;
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
	//glutAddMenuEntry("extra_visible", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}


////==============================simulation - multithread===================================//
//
////예시로 남겨놓는 과거의 흔적. 현재는 사용하지 않는다.
////함수를 너무 많이 분할해서 스레드에 할당하면, 컨텍스트 스위치가 너무 많이 일어난다.
//void fluid_simulation() {
//	thread collision_1{ &Fluid_Simulator_Grid::collision_detection_1_update, simulation };
//	thread collision_2_1{ &Fluid_Simulator_Grid::collision_detection_2_update, simulation, 0, number / 2 };
//	thread collision_2_2{ &Fluid_Simulator_Grid::collision_detection_2_update, simulation, number / 2, number };
//	collision_1.join();
//	collision_2_1.join();
//	collision_2_2.join();
//
//
//	thread collision_3{ &Fluid_Simulator_Grid::collision_detection_3_update, simulation };
//	collision_3.join();
//
//	thread collision_4{ &Fluid_Simulator_Grid::collision_detection_4_update, simulation };
//	collision_4.join();
//
//
//	thread collision_5_1{ &Fluid_Simulator_Grid::collision_detection_5_update, simulation, 0,
//		simulation->temp_particles2->size() / 2 };
//	thread collision_5_2{ &Fluid_Simulator_Grid::collision_detection_5_update, simulation, simulation->temp_particles2->size() / 2,
//		simulation->temp_particles2->size() };
//	collision_5_1.join();
//	collision_5_2.join();
//
//
//	thread boundary_particle_1_1{ &Fluid_Simulator_Grid::boundarycondition_particle_1_update, simulation, 0, number / 2 };
//	thread boundary_particle_1_2{ &Fluid_Simulator_Grid::boundarycondition_particle_1_update, simulation, number / 2, number };
//	boundary_particle_1_1.join();
//	boundary_particle_1_2.join();
//
//
//	thread advection_1_1{ &Fluid_Simulator_Grid::advection_1_update, simulation, 0, number / 2 };
//	thread advection_1_2{ &Fluid_Simulator_Grid::advection_1_update, simulation, number / 2, number };
//	advection_1_1.join();
//	advection_1_2.join();
//
//
//	thread transfer_velocity_to_grid_from_particle_1_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1_update, simulation, 0,
//		simulation->cell_number / 2 };
//	thread transfer_velocity_to_grid_from_particle_1_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1_update, simulation, simulation->cell_number / 2,
//		simulation->cell_number };
//	transfer_velocity_to_grid_from_particle_1_1.join();
//	transfer_velocity_to_grid_from_particle_1_2.join();
//
//
//	thread transfer_velocity_to_grid_from_particle_2_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2_update, simulation, 0,
//		number / 2 };
//	thread transfer_velocity_to_grid_from_particle_2_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2_update, simulation, number / 2,
//		number };
//	transfer_velocity_to_grid_from_particle_2_1.join();
//	transfer_velocity_to_grid_from_particle_2_2.join();
//
//
//	thread transfer_velocity_to_grid_from_particle_3_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3_update, simulation, 0,
//		simulation->cell_number / 2 };
//	thread transfer_velocity_to_grid_from_particle_3_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3_update, simulation, simulation->cell_number / 2,
//		simulation->cell_number };
//	transfer_velocity_to_grid_from_particle_3_1.join();
//	transfer_velocity_to_grid_from_particle_3_2.join();
//
//
//	thread transfer_velocity_to_grid_from_particle_4_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4_update, simulation, 0,
//		simulation->cell_number / 2 };
//	thread transfer_velocity_to_grid_from_particle_4_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4_update, simulation, simulation->cell_number / 2,
//		simulation->cell_number };
//	transfer_velocity_to_grid_from_particle_4_1.join();
//	transfer_velocity_to_grid_from_particle_4_2.join();
//
//
//	thread classify_cell_type_1{ &Fluid_Simulator_Grid::classify_cell_type_1_update, simulation };
//	thread add_body_force_1{ &Fluid_Simulator_Grid::add_body_force_1_update, simulation };
//	classify_cell_type_1.join();
//	add_body_force_1.join();
//
//
//	thread Adjust_velocity_1_1{ &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1_update, simulation,0, simulation->cell_number / 2 };
//	thread Adjust_velocity_1_2{ &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1_update, simulation,simulation->cell_number / 2, simulation->cell_number };
//	Adjust_velocity_1_1.join();
//	Adjust_velocity_1_2.join();
//
//
//	thread air_cell_center_point_clear_1{ &Fluid_Simulator_Grid::air_cell_center_point_clear_1_update , simulation };
//	air_cell_center_point_clear_1.join();
//
//
//	thread extrapolate_velocity_1_1{ &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1_update, simulation,0,
//		simulation->cell_number / 2 };
//	thread extrapolate_velocity_1_2{ &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1_update, simulation,simulation->cell_number / 2,
//		simulation->cell_number };
//	extrapolate_velocity_1_1.join();
//	extrapolate_velocity_1_2.join();
//
//
//	thread A_setZero_1{ &Fluid_Simulator_Grid::A_setZero_1_update ,simulation };
//	A_setZero_1.join();
//
//
//	thread pressure_solve_1_1{ &Fluid_Simulator_Grid::pressure_solve_1_update, simulation,0,
//		simulation->cell_number / 2 };
//	thread pressure_solve_1_2{ &Fluid_Simulator_Grid::pressure_solve_1_update, simulation,simulation->cell_number / 2,
//		simulation->cell_number };
//	pressure_solve_1_1.join();
//	pressure_solve_1_2.join();
//
//
//	/*thread pressure_solve_2_1{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, 0,
//		simulation->cell_number / 2 };
//	thread pressure_solve_2_2{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, simulation->cell_number / 2,
//		simulation->cell_number };
//	pressure_solve_2_1.join();
//	pressure_solve_2_2.join();*/
//	thread pressure_solve_2{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, 0, simulation->cell_number };
//	pressure_solve_2.join();
//
//
//	thread pressure_solve_3{ &Fluid_Simulator_Grid::pressure_solve_3_update, simulation };
//	pressure_solve_3.join();
//
//
//	thread pressure_solve_4_1{ &Fluid_Simulator_Grid::pressure_solve_4_update, simulation,0,
//		simulation->cell_number / 2 };
//	thread pressure_solve_4_2{ &Fluid_Simulator_Grid::pressure_solve_4_update, simulation,simulation->cell_number / 2,
//		simulation->cell_number };
//	pressure_solve_4_1.join();
//	pressure_solve_4_2.join();
//
//
//	thread boundarycondition_grid_1_1{ &Fluid_Simulator_Grid::boundarycondition_grid_1_update, simulation,0,
//		simulation->cell_number / 2 };
//	thread boundarycondition_grid_1_2{ &Fluid_Simulator_Grid::boundarycondition_grid_1_update, simulation,simulation->cell_number / 2,
//		simulation->cell_number };
//	boundarycondition_grid_1_1.join();
//	boundarycondition_grid_1_2.join();
//
//
//	thread transfer_velocity_to_particle_from_grid_1_1{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1_update, simulation,0,
//		number / 2 };
//	thread transfer_velocity_to_particle_from_grid_1_2{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1_update, simulation,number / 2,
//		number };
//	transfer_velocity_to_particle_from_grid_1_1.join();
//	transfer_velocity_to_particle_from_grid_1_2.join();
//
//
//	thread t12{ &Fluid_Simulator_Grid::swap_buffer, simulation };
//	thread t13{ &Fluid_Simulator_Grid::rendering_fluid, simulation };
//	t12.join();
//	t13.join();
//
//}