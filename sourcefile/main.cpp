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
//#include "myVector2D.h"
//#include "quadratic_particle.h"
//#include "drawgrid.h"
//#include "Simulation.h"
//
//using namespace std;
//
////window size
//int Width = 1024;
//int Height = 1024;
//
//GLuint model_view; // model-view matrix uniform shader variable location
//
//					//Projection transformation parameters
//GLuint projection; //projection matrix uniform shader variable location
//
//GLuint vao;
//GLuint vbo;
//
////time
//int time_idle;
//
////simulation ����
//Fluid_Simulator_OnlyParticle simulation;
//
////number�� �����ϸ� particle ���� �ٲ��.
//int number = 200;
//
//vector<Vector2D> points;
//vector<vec3> color;
//Box bbox;
//vector<Vector2D> grid_line;
//Vector2D box_line[4];
//
////grid_N�� �����ϸ� grid ���� �ٲ��.
//int grid_N = 10;
//
//void init(void) {
//	
//	//simulation ���� �� ���ڵ� ����
//	simulation = Fluid_Simulator_OnlyParticle(number, grid_N);
//
//	//particles���� particle���� ��ġ�� ���ͼ� points�鿡 ����, color�� black ����
//	for (int i = 0; i < number; i++) {
//		points.push_back(simulation.particles[i].Location);
//		color.push_back( vec3(0.0f, 0.0f, 0.0f) );
//		//printf("%f %f\n", points[i].x, points[i].y);
//	}
//
//	for (int i = 0; i < 4 + 4*(grid_N-1); i++) {
//		color.push_back( vec3(0.0f, 0.0f, 0.0f) );
//	}
//
//	//bbox ����
//	bbox = Box(0.0, 1.0, 0.0, 1.0);
//	box_line[0] = Vector2D(bbox.xmin, bbox.ymin);
//	box_line[1] = Vector2D(bbox.xmax, bbox.ymin);
//	box_line[2] = Vector2D(bbox.xmax, bbox.ymax);
//	box_line[3] = Vector2D(bbox.xmin, bbox.ymax);
//
//	//vector�� size�� ���ϴ� ũ�⸸ŭ �ø���.
//	for (int i = 0; i < 4 * (grid_N - 1); i++) {
//		grid_line.push_back(Vector2D(0, 0));
//	}
//
//	//grid�� ���� �׸��� �Լ�
//	make_Points_of_grids(grid_line, grid_N);
//
//	glGenVertexArrays(1, &(vao));
//	glBindVertexArray(vao);
//
//	glGenBuffers(1, &(vbo));
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line) + sizeof(Vector2D) * grid_line.size() + sizeof(Vector2D) * color.size(), NULL, GL_STATIC_DRAW);
//
//	//particle�� ������
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * number , &points[0]);
//	//box ������
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number, sizeof(box_line), box_line);
//	//grid ������
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line), sizeof(Vector2D) * grid_line.size(), &grid_line[0]);
//
//	//color �Ҵ�
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number + sizeof(box_line) + sizeof(Vector2D)* grid_line.size(), sizeof(Vector2D) * color.size(), &color[0]);
//
//	//load shaders
//	GLuint program = InitShader("vshader_2dBezier.glsl", "fshader_2dBezier.glsl");
//	glUseProgram(program);
//
//	//points memory position ( points and box )
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
//
//	//color position
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(box_line) + sizeof(grid_line)));
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
//}
//
//
//void keyboard(unsigned char key, int x, int y) {
//	float speed = 0.05;
//	switch (key) {
//	case 'q':
//		exit(EXIT_SUCCESS);
//		break;
//
//	}
//	glutPostRedisplay();
//}
//
//void idle(void)
//{
//	////����
//	//printf("\n__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//
//	//������ �и��� ���
//	time_idle = glutGet(GLUT_ELAPSED_TIME);
//
//	//0.06�� ���� particle ��ġ update
//	if (time_idle % 60 == 0) {
//		simulation.particle_simulation();
//
//		//��ġ update�� �� opengl���� ����
//		for (int i = 0; i < simulation.particles.size(); i++) {
//			points[i] = simulation.particles[i].Location;
//		}
//	}
//
//	glutPostRedisplay();
//}
//
//void display() {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//
//	//ȭ�� mapping 
//	mat4 p = Ortho2D(-1.0, 1.0, -1.0, 1.0); // ��� �׷��� ���
//	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
//
//	//�ٲ� ��ǥ �ٽ� �޸𸮿� �ֱ�
//	glBindVertexArray(vao);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector2D) * number, &points[0]);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector2D) * number, sizeof(box_line), box_line);
//
//	glPointSize(3.0);
//
//	//particle �׸���
//	glDrawArrays(GL_POINTS, 0, number);
//
//	//box �׸���
//	glLineWidth(1.0);
//	glDrawArrays(GL_LINE_LOOP, number, 4);
//
//	//grid �׸���
//	glDrawArrays(GL_LINES, number + 4, 4 * (grid_N-1));
//
//	
//
//	glBindVertexArray(0);
//	glutSwapBuffers();
//}
//
//int main(int argc, char** argv) {
//	glutInit(&argc, argv);
//
//	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
//	glutInitWindowSize(Width, Height);
//
//	glutCreateWindow("2D Particle Simulation");
//
//
//
//	glewInit();
//	init();
//
//	glutDisplayFunc(display);
//	glutIdleFunc(idle);
//	glutReshapeFunc(reshape);
//	glutKeyboardFunc(keyboard);
//	glutMainLoop();
//
//	/*delete simulation;*/
//
//	return 0;
//}