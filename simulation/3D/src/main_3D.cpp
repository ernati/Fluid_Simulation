//#include <vgl.h>
//#include <InitShader.h>
//#include <mat.h>
//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
//#include <vector>
//#include <Eigen/SparseCore>
//#include <Eigen/IterativeLinearSolvers>
//#include <Eigen/Dense>
//
//#include "../header_background/grid3D.h"
//#include "../header_background/drawgrid3D.h"
//#include "../Simulation/fluid_grid_3D.h"
//#include "../header_background/box3D.h"
//
//using namespace std;
//
////window size
//int Width = 1024;
//int Height = 1024;
//
//GLuint model_view; // model-view matrix uniform shader variable location
//
////Projection transformation parameters
//GLuint projection; //projection matrix uniform shader variable location
//
//GLuint vao;
//GLuint vbo;
//
////time
//int time_idle;
//
////simulation 선언
//Fluid_Simulator_Grid* simulation;
//
////number를 조절하면 particle 수가 바뀐다.
//int number = 50000;
//
//GLfloat radius = 3.0;
//GLfloat theta = 5.0;
//GLfloat phi = 10.0;
//
//vector<Vector3D> points;
//vector<vec3> color;
//Box3D bbox;
//vector<Vector3D>* grid_points;
//vector<Vector3D>* box_points;
//
////grid_N을 조절하면 grid 수가 바뀐다.
//int grid_N = 10;
//
////시뮬레이션 상태를 조절할 option
//int Option = 3;
//
//bool isStart = false;
//bool isFluidMode = false;
//bool isParticleMode = true;
////bool isExtrapolationCell = false;
//
//
//// Projection transformation parameters
//GLfloat fovy = 45.0; //field-of-view in y direction angle (in degrees)
//
////fovy를 증가시키면 box의 크기가 작아짐 -> fov는 x_near 와 y_max의
////거리를 나타낸다 - 즉 줌 out을 한 효과가 난다
//
//GLfloat aspect; //Viewport aspect ratio
//GLfloat zNear = 0.1, zFar = 10.0; //0.5, 3에서 practice6 때 변경
//
//
////simulation의 particle들의 위치를 points에 저장
//void pushback_SimulationPoints_to_Points() {
//	points.clear();
//	for (int i = 0; i < number; i++) {
//		points.push_back(simulation->particles[i].Location);
//	}
//}
//
//void pushback_color() {
//	color.clear();
//
//	cout << "point's size is " << points.size() << endl;
//
//	for (int i = 0; i < points.size(); i++) {
//		color.push_back(vec3(0.0f, 0.0f, 1.0f));
//	}
//
//	cout << "grid_points's size is " << grid_points->size() << endl;
//
//	for (int i = 0; i < grid_points->size(); i++) {
//		color.push_back(vec3(0.0f, 0.0f, 0.0f));
//	}
//
//	cout << "box_points's size is " << box_points->size() << endl;
//
//	for (int i = 0; i < box_points->size(); i++) {
//		color.push_back(vec3(0.0f, 0.0f, 0.0f));
//	}
//
//	cout << "fluid_cell_points's size is " << simulation->fluid_cell_center_point->size() << endl;
//
//	//fluid mode는 blue
//	for (int i = 0; i < simulation->fluid_cell_center_point->size(); i++) {
//		color.push_back(vec3(0.0f, 0.0f, 0.0f));
//	}
//
//	cout << "color's size is " << color.size() << endl;
//}
//
//void Update_Points() {
//	for (int i = 0; i < simulation->particles.size(); i++) {
//		points[i] = simulation->particles[i].Location;
//	}
//}
//
//void init(void) {
//
//	//simulation 실행 및 입자들 생성
//	simulation = new Fluid_Simulator_Grid(number, grid_N);
//
//	pushback_SimulationPoints_to_Points();
//
//	//bbox 선언
//	bbox = Box3D(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
//	box_points = bbox.make_box_points(); // 
//
//	//grid의 점을 그리는 함수
//	grid_points = make_Points_of_grids(grid_N);
//
//	grid_points->clear();
//
//	pushback_color();
//
//	glGenVertexArrays(1, &(vao));
//	glBindVertexArray(vao);
//
//	glGenBuffers(1, &(vbo));
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D)* box_points->size() + sizeof(Vector3D) * grid_points->size() + sizeof(Vector3D) * simulation->fluid_cell_center_point->size() + sizeof(Vector3D) * color.size(), NULL, GL_STATIC_DRAW);
//	
//	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size() + sizeof(Vector3D) * color.size(), NULL, GL_STATIC_DRAW);
//
//	//particle들 렌더링
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3D) * points.size(), &points[0]);
//	
//	////box 렌더링
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size(), sizeof(Vector3D) * box_points->size(), &((*box_points)[0]));
//	  
//	//grid 렌더링
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size(), sizeof(Vector3D) * grid_points->size(), &((*grid_points)[0]) );
//
//	//fluid cell center point 렌더링
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size(), sizeof(Vector3D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));
//
//	////air cell center point 렌더링
//	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * points.size() + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * simulation->fluid_cell_center_point->size(), sizeof(Vector2D) *
//	//	simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));
//
//	////color 할당
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size() + sizeof(Vector3D) * simulation->fluid_cell_center_point->size(), sizeof(vec3) * color.size(), &color[0]);
//
//	//load shaders
//	GLuint program = InitShader("simulation/3D/src/vshader_2dBezier.glsl", "simulation/3D/src/fshader_2dBezier.glsl");
//	glUseProgram(program);
//
//	//points memory position ( points and box )
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), BUFFER_OFFSET(0));
//
//	//color position
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), BUFFER_OFFSET(sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size() + sizeof(Vector3D) * simulation->fluid_cell_center_point->size() ));
//
//	glEnableVertexAttribArray(0);
//	//initialize uniform variable from vertex shander
//	model_view = glGetUniformLocation(program, "model_view");
//	projection = glGetUniformLocation(program, "projection");
//
//	glEnable(GL_DEPTH_TEST);
//	glClearColor(1.0, 1.0, 1.0, 0.0);
//}
//
//void reshape(int width, int height)
//{
//	glViewport(0, 0, width, height);
//	aspect = GLfloat(width) / height;
//}
//
//
//double xxx = 0;
//
//void keyboard(unsigned char key, int x, int y) {
//	switch (key) {
//	case 'q':
//		//simulation->delete_vectors();
//		//delete simulation;
//		exit(EXIT_SUCCESS);
//		break;
//
//
//	case 'z': zNear *= 1.1; zFar *= 1.1; break;
//	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
//	case 'r': radius *= 2.0; break;
//	case 'R': radius *= 0.5; break;
//	case 'o': theta += 5.0; break;
//	case 'O': theta -= 5.0; break;
//	case 'p': phi += 5.0; break;
//	case 'P': phi -= 5.0; break;
//		
//	case ' ':  // reset values to their defaults
//		zNear = 0.1;
//		zFar = 10.0;
//		
//		radius = 3.0;
//		theta = 5.0;
//		phi = 10.0;
//		break;
//
//	}
//
//	glutPostRedisplay();
//}
//
//void idle(void)
//{
//	////구분
//	//printf("\n__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//
//	//누적된 밀리초 얻기
//	time_idle = glutGet(GLUT_ELAPSED_TIME);
//
//	//0.06초 마다 particle 위치 update
//	if (time_idle % 60 == 0) {
//		if (isStart) {
//			simulation->particle_simulation();
//			Update_Points();
//		}
//	}
//
//	glutPostRedisplay();
//}
//void display() {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	vec4 eye(0.5 + radius * sin(theta) * cos(phi),
//			0.5 + radius * sin(theta) * sin(phi),
//			0.5 + radius * cos(theta),
//			1.0);
//    vec4 at(0.5, 0.5, 0.5, 1.0);
//    vec4 up(0.0, 1.0, 0.0, 0.0);
//
//    mat4 mv = LookAt(eye, at, up);
//    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
//
//    mat4 p = Perspective(fovy, aspect, zNear, zFar);
//    glUniformMatrix4fv(projection, 1, GL_TRUE, p);
//
//
//	//바뀐 좌표 다시 메모리에 넣기
//	glBindVertexArray(vao);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3D) * points.size(), &points[0]);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size(), sizeof(Vector3D) * simulation->fluid_cell_center_point->size(), &((*simulation->fluid_cell_center_point)[0]));
//	/*glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3D) * points.size() + sizeof(Vector3D) * box_points->size() + sizeof(Vector3D) * grid_points->size() + sizeof(Vector3D) * simulation->fluid_cell_center_point->size(), sizeof(Vector3D) *
//					simulation->air_cell_center_point->size(), &((*simulation->air_cell_center_point)[0]));*/
//
//	glPointSize(5.0);
//
//	if (isStart) {
//		if (isParticleMode) { glDrawArrays(GL_POINTS, 0, points.size()); }
//	}
//
//	//box 그리기
//	glLineWidth(2.0);
//	glDrawArrays(GL_LINE_LOOP, points.size(), 4); // 정사각형 1
//	glDrawArrays(GL_LINE_LOOP, points.size()+4, 4);
//	glDrawArrays(GL_LINE_LOOP, points.size()+8, 4);
//	glDrawArrays(GL_LINE_LOOP, points.size()+12, 4); // 정사각형 4
//
//	////grid 그리기
//	//glDrawArrays(GL_LINES, points.size() + 16, 4 * (grid_N + 1) * (grid_N + 1));
//
//	glPointSize(10.0);
//	if (isStart) {
//		if (isFluidMode) {
//			glDrawArrays(GL_POINTS, points.size() + 16 , simulation->fluid_cell_center_point->size());
//		}
//		//if (isExtrapolationCell) {
//		//	//glLineWidth(0.05);
//		//	//glDrawArrays(GL_LINES, points.size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
//
//		//	glDrawArrays(GL_POINTS, points.size() + 4 + 4 * (grid_N - 1) + simulation->fluid_cell_center_point->size(), simulation->air_cell_center_point->size());
//		//}
//	}
//
//	glBindVertexArray(0);
//	glutSwapBuffers();
//}
//
//void Menu(int Option) {
//
//	switch (Option) {
//		//particlemode
//	case 0:
//		isParticleMode = true;
//		isFluidMode = false;
//		break;
//
//		//fluidmode
//	case 1:
//		isParticleMode = false;
//		isFluidMode = true;
//		break;
//
//		//start
//	case 2:
//		isStart = true;
//		break;
//
//		//finish
//	case 3:
//		isStart = false;
//		simulation->clear_and_ReInit();
//		break;
//
//	//	//extra_visible
//	//case 4:
//	//	isExtrapolationCell = !isExtrapolationCell;
//	//	break;
//
//	}
//}
//
//int main(int argc, char** argv) {
//	glutInit(&argc, argv);
//
//	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
//	glutInitWindowSize(Width, Height);
//
//	aspect = 1024.0 / 1024.0;
//
//	glutCreateWindow("3D Particle Simulation");
//
//	glewInit();
//	init();
//
//	glutDisplayFunc(display);
//	glutIdleFunc(idle);
//	glutReshapeFunc(reshape);
//	glutKeyboardFunc(keyboard);
//
//	glutCreateMenu(Menu);
//	glutAddMenuEntry("ParticleMode", 0);
//	glutAddMenuEntry("FluidMode", 1);
//	glutAddMenuEntry("Start", 2);
//	glutAddMenuEntry("Finish", 3);
//	//glutAddMenuEntry("extra_visible", 4);
//	glutAttachMenu(GLUT_RIGHT_BUTTON);
//
//	glutMainLoop();
//
//	return 0;
//}