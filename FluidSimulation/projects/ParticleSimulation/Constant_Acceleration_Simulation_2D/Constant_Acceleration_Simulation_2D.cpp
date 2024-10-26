#include "Simulation/Constant_Acceleration_Simulation_2D.h"


int main(void)
{
	int N = 1000;
	int grid_N = 20;

	int i = 0;
	//1. 입자를 담을 벡터 생성 및 시뮬레이션 클래스 인스턴스 생성
	Constant_Acceleration_Simulator simulation = Constant_Acceleration_Simulator( N , grid_N );

	//2. simulation 1회 시도
	while (true) {
		if (i % 1000000000 == 0) {
			simulation.particle_simulation();

			//3. simulation 결과 출력
			for (int i = 0; i < N; i++) {
				printf("(%f,%f) ", simulation.particles[i].Location.X, simulation.particles[i].Location.Y);
				if (i % 10 == 9) printf("\n");
			}
			printf("\n===========================================\n");
		}
		i++;
	}
}