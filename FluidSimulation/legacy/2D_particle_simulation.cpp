//#include <vgl.h>
//#include <InitShader.h>
//#include <mat.h>
//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
//
//#include "2D_particle_simulation.h"
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
//
//
//int number = 1000;
//vec2 points[1000];
//Particle particles[1000];
//vec3 color[1000];
//Box bbox;
//vec2 box_line[4];
//
//void init(void) {
//	
//	// particles 선언
//	srand((unsigned int)time(NULL));
//	for (int i = 0; i < number; i++) {
//		float a =  (float)(rand()  % Width) / Width;
//		float b = (float)(rand() % Height) / Height;
//		particles[i] = Particle(1.0, vec2(a, b), vec2(a / 1000 ,b / 1000));
//	}
//	
//	//particles에서 particle들의 위치만 뺴와서 points들에 저장, color는 black 고정
//	for (int i = 0; i < number; i++) {
//		points[i] = particles[i].Location;
//		color[i] = vec3( 0.0f, 0.0f, 0.0f );
//		//printf("%f %f\n", points[i].x, points[i].y);
//	}
//
//	//bbox 선언
//	bbox = Box(0.0, 1.0, 0.0, 1.0);
//	box_line[0] = vec2(bbox.xmin, bbox.ymin);
//	box_line[1] = vec2(bbox.xmax, bbox.ymin);
//	box_line[2] = vec2(bbox.xmax, bbox.ymax);
//	box_line[3] = vec2(bbox.xmin, bbox.ymax);
//	
//
//	glGenVertexArrays(1, &(vao));
//	glBindVertexArray(vao);
//
//	glGenBuffers(1, &(vbo));
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(color) + sizeof(box_line), NULL, GL_STATIC_DRAW);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(box_line), box_line);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(box_line), sizeof(color), color);
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
//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET( sizeof(points) + sizeof(box_line) ) ) ;
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
//	////구분
//	//printf("\n__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//	//printf("__________________________________________________________________________________\n");
//
//	//입자 움직임
//	for (int i = 0; i < number; i++) {
//		/*printf("%f %f -> ", points[i].x, points[i].y);*/
//		particles[i].move();
//		points[i] = particles[i].Location;
//		//printf("%f %f\n", points[i].x, points[i].y);
//	}
//
//        glutPostRedisplay();
//    }
//
//void display() {
//	glClear(GL_COLOR_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClear(GL_COLOR_BUFFER_BIT);
//
//
//	//화면 mapping 
//	mat4 p = Ortho2D(-1.0, 1.0, -1.0, 1.0); // 곡선이 그려질 평면
//	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
//
//	//바뀐 좌표 다시 메모리에 넣기
//	glBindVertexArray(vao);
//	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
//	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(box_line), box_line);
//
//	glPointSize(1.5);
//
//	for (int i = 0; i < number; i++) {
//		glDrawArrays(GL_POINTS, i, 1);
//	}
//
//	glLineWidth(2.0);
//	glDrawArrays(GL_LINE_LOOP, number, 4);
//
//	glBindVertexArray(0);
//	glutSwapBuffers();
// Note: Main function commented out as this is legacy code
// The active main function is in the visualization/opengl files