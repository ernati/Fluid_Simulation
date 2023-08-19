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

#include "myVector2D.h"
#include "quadratic_particle.h"
#include "drawgrid.h"
#include "Simulation.h"

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

//number를 조절하면 particle 수가 바뀐다.
int number = 6000;

vector<Vector2D> points;
vector<vec3> color;
Box bbox;
vector<Vector2D> grid_line;
Vector2D box_line[4];

//grid_N을 조절하면 grid 수가 바뀐다.
int grid_N = 40;

//시뮬레이션 상태를 조절할 option
int Option = 3;

bool isStart = false;
bool isFluidMode = false;
bool isParticleMode = true;

//simulation의 particle들의 위치를 points에 저장
void pushback_SimulationPoints_to_Points() {
	points.clear();
	for (int i = 0; i < number; i++) {
		points.push_back(simulation->particles[i].Location);
	}
}

void pushback_color() {
	color.clear();

	for (int i = 0; i < number; i++) {
		color.push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	for (int i = 0; i < 4 + 4 * (grid_N - 1); i++) {
		color.push_back(vec3(0.0f, 0.0f, 0.0f));
	}

	//fluid mode는 blue
	for (int i = 0; i < simulation->fluid_cell_center_point->size(); i++) {
		color.push_back(vec3(0.0f, 0.0f, 1.0f));
	}
}

void Update_Points() {
	for (int i = 0; i < simulation->particles.size(); i++) {
			points[i] = simulation->particles[i].Location;
	}
}

void init(void) {
	
	//simulation 실행 및 입자들 생성
	simulation = new Fluid_Simulator_Grid(number, grid_N);

	pushback_SimulationPoints_to_Points();

	pushback_color();

	//bbox 선언
	bbox = Box(0.0, 1.0, 0.0, 1.0);
	box_line[0] = Vector2D(bbox.xmin, bbox.ymin);
	box_line[1] = Vector2D(bbox.xmax, bbox.ymin);
	box_line[2] = Vector2D(bbox.xmax, bbox.ymax);
	box_line[3] = Vector2D(bbox.xmin, bbox.ymax);

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
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * color.size(), NULL, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size() + sizeof(Vector2D) * color.size(), NULL, GL_STATIC_DRAW);

	//particle들 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * points.size() , &points[0]);
	//box 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size(), sizeof(box_line), box_line);
	//grid 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line), sizeof(Vector2D) * grid_line.size(), &grid_line[0]);

	//fluid cell center point 렌더링
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line) + sizeof(Vector2D)* grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));

	//color 할당
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line) + sizeof(Vector2D)* grid_line.size(), sizeof(Vector2D) * color.size(), &color[0]);

	//load shaders
	GLuint program = InitShader("vshader_2dBezier.glsl", "fshader_2dBezier.glsl");
	glUseProgram(program);

	//points memory position ( points and box )
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//color position
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(box_line) + sizeof(grid_line)));

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
	if (time_idle % 60 == 0) {
		if (isStart) {
			simulation->particle_simulation();
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * points.size(), &points[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size(), sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));

	glPointSize(3.0);
	
	if (isStart) {
		if (isParticleMode) { glDrawArrays(GL_POINTS, 0, points.size()); }
	}

	//box 그리기
	glLineWidth(0.1);
	glDrawArrays(GL_LINE_LOOP, points.size(), 4);

	//grid 그리기
	glDrawArrays(GL_LINES, points.size() + 4, 4 * (grid_N-1));

	glPointSize(10.0);
	if (isStart) {
		if (isFluidMode) { glDrawArrays(GL_POINTS, points.size() + 4 + 4 * (grid_N - 1), simulation->fluid_cell_center_point->size()); }
	}
	

	

	glBindVertexArray(0);
	glutSwapBuffers();
}

void Menu(int Option) {

	switch (Option) {
		//particlemode
		case 0 :
			isParticleMode = true;
			isFluidMode = false;
			break;

		//fluidmode
		case 1 :
			isParticleMode = false;
			isFluidMode = true;
			break;

		//start
		case 2 : 
			isStart = true;
			break;

		//finish
		case 3 :
			isStart = false;
			simulation->clear_and_ReInit();
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
	glutAddMenuEntry("FluidMode", 1);
	glutAddMenuEntry("Start", 2);
	glutAddMenuEntry("Finish", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}