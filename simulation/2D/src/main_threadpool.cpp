#pragma once
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

//#include "../../threadpool/threadpool_simulation.h"

#include "../header_background/grid.h"
#include "../header_background/drawgrid.h"
#include "../Simulation/fluid_grid_2D_multithread.h"
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

//simulation 선언
Fluid_Simulator_Grid* simulation;

////thread_pool
//std::unique_ptr<thread_pool_simulation> threadpool;

//number를 조절하면 particle 수가 바뀐다.
int number = 8000;

vector<Vector2D>* points;
vector<vec3>* color;
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
bool isExtrapolationCell = false;

//simulation의 particle들의 위치를 points에 저장
void pushback_SimulationPoints_to_Points() {
	points->clear();
	for (int i = 0; i < number; i++) {
		points->push_back(simulation->particles[i].Location);
	}

}

void pushback_color() {
	color->clear();

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

}

void Update_Points() {
	for (int i = 0; i < simulation->particles.size(); i++) {
		(*points)[i] = simulation->particles[i].Location;
	}
}

void init(void) {

	points = new vector<Vector2D>();
	color = new vector<vec3>();

	//simulation 실행 및 입자들 생성
	simulation = new Fluid_Simulator_Grid(number, grid_N);

	pushback_SimulationPoints_to_Points();
	//threadpool->thread_pool_submit_void(pushback_SimulationPoints_to_Points);

	pushback_color();

	Sleep(2000);

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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * color->size(), NULL, GL_STATIC_DRAW);

	//particle들 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * points->size(), &((*points)[0]));
	//box 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size(), sizeof(Vector2D) * box_line.size(), &(box_line[0]));
	//grid 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size(), sizeof(Vector2D) * grid_line.size(), &(grid_line[0]));

	//fluid cell center point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));

	////air cell center point 렌더링
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * 
	//	simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));

	//color 할당
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) * color->size(), &((*color)[0]));

	//load shaders
	GLuint program = InitShader("simulation/2D/src/vshader_2dBezier.glsl", "simulation/2D/src/fshader_2dBezier.glsl");
	glUseProgram(program);

	//points memory position ( points and box )
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), BUFFER_OFFSET(0));

	//color position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), BUFFER_OFFSET(sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size()));

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
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		simulation->delete_vectors();
		delete simulation;
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

			/*thread t1 = thread( &Fluid_Simulator_Grid::particle_simulation, simulation );
			t1.join();*/

			std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

			////0. promise and future
			//promise<void> myPromise;
			//auto theFuture = myPromise.get_future();


			////1. 예상 위치 계산
			//simulation->temp_particles->clear();
			//thread collision_1{ &Fluid_Simulator_Grid::collision_detection_1, simulation, move(myPromise) };
			//thread collision_2_1{ &Fluid_Simulator_Grid::collision_detection_2, simulation, 0, number/2, move(myPromise) };
			//thread collision_2_2{ &Fluid_Simulator_Grid::collision_detection_2, simulation, number/2, number, move(myPromise) };
			//collision_1.join();
			//collision_2_1.join();
			//collision_2_2.join();


			//promise<void> myPromise2;
			//auto theFuture2 = myPromise2.get_future();
			//thread collision_3{ &Fluid_Simulator_Grid::collision_detection_3, simulation, move(myPromise2) };
			//collision_3.join();


			//simulation->temp_particles2->clear();
			//promise<void> myPromise3;
			//auto theFuture3 = myPromise3.get_future();
			//thread collision_4{ &Fluid_Simulator_Grid::collision_detection_4, simulation, move(myPromise3) };
			//collision_4.join();


			//promise<void> myPromise4;
			//auto theFuture4 = myPromise4.get_future();
			//thread collision_5_1{ &Fluid_Simulator_Grid::collision_detection_5, simulation, 0, 
			//	simulation->temp_particles2->size()/2, move(myPromise4) };
			//thread collision_5_2{ &Fluid_Simulator_Grid::collision_detection_5, simulation, simulation->temp_particles2->size() / 2,
			//	simulation->temp_particles2->size(), move(myPromise4) };
			//collision_5_1.join();
			//collision_5_2.join();


			//promise<void> myPromise5;
			//auto theFuture5 = myPromise5.get_future();
			//thread boundary_particle_1_1 { &Fluid_Simulator_Grid::boundarycondition_particle_1, simulation, 0, number/2,move(myPromise5) };
			//thread boundary_particle_1_2 { &Fluid_Simulator_Grid::boundarycondition_particle_1, simulation, number/2, number,move(myPromise5) };
			//boundary_particle_1_1.join();
			//boundary_particle_1_2.join();
			//

			//promise<void> myPromise6;
			//auto theFuture6 = myPromise6.get_future();
			//thread advection_1_1{ &Fluid_Simulator_Grid::advection_1, simulation, 0, number / 2,move(myPromise6) };
			//thread advection_1_2{ &Fluid_Simulator_Grid::advection_1, simulation, number / 2, number,move(myPromise6) };
			//advection_1_1.join();
			//advection_1_2.join();


			//promise<void> myPromise7;
			//auto theFuture7 = myPromise7.get_future();
			//thread transfer_velocity_to_grid_from_particle_1_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1, simulation, 0,
			//	simulation->cell_number / 2,move(myPromise7) };
			//thread transfer_velocity_to_grid_from_particle_1_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1, simulation, simulation->cell_number / 2,
			//	simulation->cell_number,move(myPromise7) };
			//transfer_velocity_to_grid_from_particle_1_1.join();
			//transfer_velocity_to_grid_from_particle_1_2.join();


			//promise<void> myPromise8;
			//auto theFuture8 = myPromise8.get_future();
			//thread transfer_velocity_to_grid_from_particle_2_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2, simulation, 0,
			//	number / 2,move(myPromise8) };
			//thread transfer_velocity_to_grid_from_particle_2_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2, simulation, number / 2,
			//	number,move(myPromise8) };
			//transfer_velocity_to_grid_from_particle_2_1.join();
			//transfer_velocity_to_grid_from_particle_2_2.join();


			//promise<void> myPromise9;
			//auto theFuture9= myPromise9.get_future();
			//thread transfer_velocity_to_grid_from_particle_3_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3, simulation, 0,
			//	simulation->cell_number / 2,move(myPromise9) };
			//thread transfer_velocity_to_grid_from_particle_3_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3, simulation, simulation->cell_number / 2,
			//	simulation->cell_number,move(myPromise9) };
			//transfer_velocity_to_grid_from_particle_3_1.join();
			//transfer_velocity_to_grid_from_particle_3_2.join();
			//

			//promise<void> myPromise10;
			//auto theFuture10 = myPromise10.get_future();
			//thread transfer_velocity_to_grid_from_particle_4_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4, simulation, 0,
			//	simulation->cell_number / 2,move(myPromise10) };
			//thread transfer_velocity_to_grid_from_particle_4_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4, simulation, simulation->cell_number / 2,
			//	simulation->cell_number,move(myPromise10) };
			//transfer_velocity_to_grid_from_particle_4_1.join();
			//transfer_velocity_to_grid_from_particle_4_2.join();
			//

			//promise<void> myPromise11;
			//auto theFuture11 = myPromise11.get_future();
			//thread classify_cell_type_1 { &Fluid_Simulator_Grid::classify_cell_type_1, simulation, move(myPromise11) };
			//thread add_body_force_1 { &Fluid_Simulator_Grid::add_body_force_1, simulation, move(myPromise11) };
			//classify_cell_type_1.join();
			//add_body_force_1.join();
			//

			//promise<void> myPromise12;
			//auto theFuture12 = myPromise12.get_future();
			//thread Adjust_velocity_1_1 { &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1, simulation,0, simulation->cell_number / 2, move(myPromise12) };
			//thread Adjust_velocity_1_2 { &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1, simulation,simulation->cell_number / 2, simulation->cell_number, move(myPromise12) };
			//Adjust_velocity_1_1.join();
			//Adjust_velocity_1_2.join();


			//promise<void> myPromise121;
			//auto theFuture121 = myPromise121.get_future();
			//thread air_cell_center_point_clear_1 { &Fluid_Simulator_Grid::air_cell_center_point_clear_1 , simulation, move(myPromise121) };
			//air_cell_center_point_clear_1.join();


			//promise<void> myPromise13;
			//auto theFuture13 = myPromise13.get_future();
			//thread extrapolate_velocity_1_1 { &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1, simulation,0, 
			//	simulation->cell_number / 2,  move(myPromise13) };
			//thread extrapolate_velocity_1_2{ &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1, simulation,simulation->cell_number / 2, 
			//	simulation->cell_number,  move(myPromise13) };
			//extrapolate_velocity_1_1.join();
			//extrapolate_velocity_1_2.join();


			//promise<void> myPromise131;
			//auto theFuture131 = myPromise131.get_future();
			//thread A_setZero_1 { &Fluid_Simulator_Grid::A_setZero_1 ,simulation, move(myPromise131) };
			//A_setZero_1.join();
			//

			//promise<void> myPromise14;
			//auto theFuture14 = myPromise14.get_future();
			//thread pressure_solve_1_1{ &Fluid_Simulator_Grid::pressure_solve_1, simulation,0,
			//	simulation->cell_number / 2,  move(myPromise14) };
			//thread pressure_solve_1_2{ &Fluid_Simulator_Grid::pressure_solve_1, simulation,simulation->cell_number / 2,
			//	simulation->cell_number,  move(myPromise14) };
			//pressure_solve_1_1.join();
			//pressure_solve_1_2.join();


			//promise<void> myPromise15;
			//auto theFuture15 = myPromise15.get_future();
			////simulation->pressure_solve_2( 0, simulation->cell_number, move(myPromise15) );
			//thread pressure_solve_2_1{ &Fluid_Simulator_Grid::pressure_solve_2, simulation, 0,
			//	simulation->cell_number / 2,  move(myPromise15) };
			//thread pressure_solve_2_2{ &Fluid_Simulator_Grid::pressure_solve_2, simulation, simulation->cell_number / 2,
			//	simulation->cell_number,  move(myPromise15) };
			//pressure_solve_2_1.join();
			//pressure_solve_2_2.join();


			//promise<void> myPromise16;
			//auto theFuture16 = myPromise16.get_future();
			////simulation->pressure_solve_3( move(myPromise16) );
			//thread pressure_solve_3{ &Fluid_Simulator_Grid::pressure_solve_3, simulation, move(myPromise16) };
			//pressure_solve_3.join();


			//promise<void> myPromise17;
			//auto theFuture17 = myPromise17.get_future();
			//thread pressure_solve_4_1{ &Fluid_Simulator_Grid::pressure_solve_4, simulation,0,
			//	simulation->cell_number / 2,  move(myPromise17) };
			//thread pressure_solve_4_2{ &Fluid_Simulator_Grid::pressure_solve_4, simulation,simulation->cell_number / 2,
			//	simulation->cell_number,  move(myPromise17) };
			//pressure_solve_4_1.join();
			//pressure_solve_4_2.join();
			//

			//promise<void> myPromise18;
			//auto theFuture18 = myPromise18.get_future();
			//thread boundarycondition_grid_1_1{ &Fluid_Simulator_Grid::boundarycondition_grid_1, simulation,0,
			//	simulation->cell_number / 2,  move(myPromise18) };
			//thread boundarycondition_grid_1_2{ &Fluid_Simulator_Grid::boundarycondition_grid_1, simulation,simulation->cell_number / 2,
			//	simulation->cell_number,  move(myPromise18) };
			//boundarycondition_grid_1_1.join();
			//boundarycondition_grid_1_2.join();
			//

			//promise<void> myPromise19;
			//auto theFuture19 = myPromise19.get_future();
			//thread transfer_velocity_to_particle_from_grid_1_1{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1, simulation,0,
			//	number / 2,  move(myPromise19) };
			//thread transfer_velocity_to_particle_from_grid_1_2{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1, simulation,number / 2,
			//	number,  move(myPromise19) };
			//transfer_velocity_to_particle_from_grid_1_1.join();
			//transfer_velocity_to_particle_from_grid_1_2.join();
			//

			//promise<void> myPromise20;
			//auto theFuture20 = myPromise20.get_future();
			//thread t12{ &Fluid_Simulator_Grid::swap_buffer, simulation, move(myPromise20) };
			//thread t13{ &Fluid_Simulator_Grid::rendering_fluid, simulation, move(myPromise20) };
			//t12.join();
			//t13.join();

			//========================================================================================================
			

			thread collision_1{ &Fluid_Simulator_Grid::collision_detection_1_update, simulation };
			thread collision_2_1{ &Fluid_Simulator_Grid::collision_detection_2_update, simulation, 0, number/2 };
			thread collision_2_2{ &Fluid_Simulator_Grid::collision_detection_2_update, simulation, number/2, number };
			collision_1.join();
			collision_2_1.join();
			collision_2_2.join();


			thread collision_3{ &Fluid_Simulator_Grid::collision_detection_3_update, simulation };
			collision_3.join();

			thread collision_4 { &Fluid_Simulator_Grid::collision_detection_4_update, simulation };
			collision_4.join();


			thread collision_5_1{ &Fluid_Simulator_Grid::collision_detection_5_update, simulation, 0,
				simulation->temp_particles2->size()/2 };
			thread collision_5_2{ &Fluid_Simulator_Grid::collision_detection_5_update, simulation, simulation->temp_particles2->size() / 2,
				simulation->temp_particles2->size() };
			collision_5_1.join();
			collision_5_2.join();


			thread boundary_particle_1_1 { &Fluid_Simulator_Grid::boundarycondition_particle_1_update, simulation, 0, number/2 };
			thread boundary_particle_1_2 { &Fluid_Simulator_Grid::boundarycondition_particle_1_update, simulation, number/2, number };
			boundary_particle_1_1.join();
			boundary_particle_1_2.join();


			thread advection_1_1{ &Fluid_Simulator_Grid::advection_1_update, simulation, 0, number / 2 };
			thread advection_1_2{ &Fluid_Simulator_Grid::advection_1_update, simulation, number / 2, number };
			advection_1_1.join();
			advection_1_2.join();


			thread transfer_velocity_to_grid_from_particle_1_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1_update, simulation, 0,
				simulation->cell_number / 2 };
			thread transfer_velocity_to_grid_from_particle_1_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_1_update, simulation, simulation->cell_number / 2,
				simulation->cell_number };
			transfer_velocity_to_grid_from_particle_1_1.join();
			transfer_velocity_to_grid_from_particle_1_2.join();


			thread transfer_velocity_to_grid_from_particle_2_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2_update, simulation, 0,
				number / 2 };
			thread transfer_velocity_to_grid_from_particle_2_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_2_update, simulation, number / 2,
				number };
			transfer_velocity_to_grid_from_particle_2_1.join();
			transfer_velocity_to_grid_from_particle_2_2.join();


			thread transfer_velocity_to_grid_from_particle_3_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3_update, simulation, 0,
				simulation->cell_number / 2 };
			thread transfer_velocity_to_grid_from_particle_3_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_3_update, simulation, simulation->cell_number / 2,
				simulation->cell_number };
			transfer_velocity_to_grid_from_particle_3_1.join();
			transfer_velocity_to_grid_from_particle_3_2.join();


			thread transfer_velocity_to_grid_from_particle_4_1{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4_update, simulation, 0,
				simulation->cell_number / 2 };
			thread transfer_velocity_to_grid_from_particle_4_2{ &Fluid_Simulator_Grid::transfer_velocity_to_grid_from_particle_4_update, simulation, simulation->cell_number / 2,
				simulation->cell_number };
			transfer_velocity_to_grid_from_particle_4_1.join();
			transfer_velocity_to_grid_from_particle_4_2.join();


			thread classify_cell_type_1 { &Fluid_Simulator_Grid::classify_cell_type_1_update, simulation };
			thread add_body_force_1 { &Fluid_Simulator_Grid::add_body_force_1_update, simulation };
			classify_cell_type_1.join();
			add_body_force_1.join();


			thread Adjust_velocity_1_1 { &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1_update, simulation,0, simulation->cell_number / 2 };
			thread Adjust_velocity_1_2 { &Fluid_Simulator_Grid::Adjust_velocity_from_bodyforce_1_update, simulation,simulation->cell_number / 2, simulation->cell_number };
			Adjust_velocity_1_1.join();
			Adjust_velocity_1_2.join();


			thread air_cell_center_point_clear_1 { &Fluid_Simulator_Grid::air_cell_center_point_clear_1_update , simulation };
			air_cell_center_point_clear_1.join();


			thread extrapolate_velocity_1_1 { &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1_update, simulation,0,
				simulation->cell_number / 2 };
			thread extrapolate_velocity_1_2 { &Fluid_Simulator_Grid::extrapolate_velocity_to_air_cell_1_update, simulation,simulation->cell_number / 2,
				simulation->cell_number };
			extrapolate_velocity_1_1.join();
			extrapolate_velocity_1_2.join();


			thread A_setZero_1 { &Fluid_Simulator_Grid::A_setZero_1_update ,simulation };
			A_setZero_1.join();


			thread pressure_solve_1_1{ &Fluid_Simulator_Grid::pressure_solve_1_update, simulation,0,
				simulation->cell_number / 2 };
			thread pressure_solve_1_2{ &Fluid_Simulator_Grid::pressure_solve_1_update, simulation,simulation->cell_number / 2,
				simulation->cell_number };
			pressure_solve_1_1.join();
			pressure_solve_1_2.join();


			/*thread pressure_solve_2_1{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, 0,
				simulation->cell_number / 2 };
			thread pressure_solve_2_2{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, simulation->cell_number / 2,
				simulation->cell_number };
			pressure_solve_2_1.join();
			pressure_solve_2_2.join();*/
			thread pressure_solve_2{ &Fluid_Simulator_Grid::pressure_solve_2_update, simulation, 0, simulation->cell_number };
			pressure_solve_2.join();


			thread pressure_solve_3{ &Fluid_Simulator_Grid::pressure_solve_3_update, simulation };
			pressure_solve_3.join();


			thread pressure_solve_4_1{ &Fluid_Simulator_Grid::pressure_solve_4_update, simulation,0,
				simulation->cell_number / 2 };
			thread pressure_solve_4_2{ &Fluid_Simulator_Grid::pressure_solve_4_update, simulation,simulation->cell_number / 2,
				simulation->cell_number };
			pressure_solve_4_1.join();
			pressure_solve_4_2.join();


			thread boundarycondition_grid_1_1{ &Fluid_Simulator_Grid::boundarycondition_grid_1_update, simulation,0,
				simulation->cell_number / 2 };
			thread boundarycondition_grid_1_2{ &Fluid_Simulator_Grid::boundarycondition_grid_1_update, simulation,simulation->cell_number / 2,
				simulation->cell_number };
			boundarycondition_grid_1_1.join();
			boundarycondition_grid_1_2.join();


			thread transfer_velocity_to_particle_from_grid_1_1{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1_update, simulation,0,
				number / 2 };
			thread transfer_velocity_to_particle_from_grid_1_2{ &Fluid_Simulator_Grid::transfer_Velocity_to_particle_from_grid_1_update, simulation,number / 2,
				number };
			transfer_velocity_to_particle_from_grid_1_1.join();
			transfer_velocity_to_particle_from_grid_1_2.join();


			thread t12{ &Fluid_Simulator_Grid::swap_buffer, simulation };
			thread t13{ &Fluid_Simulator_Grid::rendering_fluid, simulation };
			t12.join();
			t13.join();


			std::chrono::duration<double>sec = std::chrono::system_clock::now() - start;
			std::cout << "simulation 걸리는 시간(초) : " << sec.count() << "seconds" << std::endl;


			Update_Points();
		}
	}

	glutPostRedisplay();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);


	//화면 mapping 
	mat4 p = Ortho2D(-1.0, 1.0, -1.0, 1.0); // 곡선이 그려질 평면
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	//바뀐 좌표 다시 메모리에 넣기
	glBindVertexArray(vao);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * points->size(), &((*points)[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));
	/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points->size() + sizeof(Vector2D) * box_line.size() + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) *
		simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));*/

	glPointSize(2.0);

	if (isStart) {
		if (isParticleMode) { glDrawArrays(GL_POINTS, 0, points->size()); }
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, points->size(), 4);

	//grid 그리기
	//glDrawArrays(GL_LINES, points->size() + 4, 4 * (grid_N-1));

	glPointSize(10.0);
	if (isStart) {
		if (isPixelMode) {
			glDrawArrays(GL_POINTS, points->size() + 4 + 4 * (grid_N - 1), simulation->fluid_cell_center_point->size());
		}
		if (isExtrapolationCell) {
			//glLineWidth(0.05);
			//glDrawArrays(GL_LINES, points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());

			glDrawArrays(GL_POINTS, points->size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
		}
	}

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

		//PixelMode
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
	case 4:
		isExtrapolationCell = !isExtrapolationCell;
		break;

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
	glutAddMenuEntry("extra_visible", 4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}