#include "Simulation/simul_sinecosine.h"


int main(void)
{
	int N = 1000;
	int grid_N = 20;

	int i = 0;
	//1. ���ڸ� ���� ���� ���� �� �ùķ��̼� Ŭ���� �ν��Ͻ� ����
	Simul_SineCosine simulation = Simul_SineCosine(N);

	//2. simulation 1ȸ �õ�
	while (true) {
		if (i % 1000000000 == 0) {
			simulation.particle_simulation();

			//3. simulation ��� ���
			for (int i = 0; i < N; i++) {
				printf("(%f,%f) ", (*simulation.sine_particles)[i].X, (*simulation.sine_particles)[i].Y);
				printf("(%f,%f) ", (*simulation.cosine_particles)[i].X, (*simulation.cosine_particles)[i].Y);
				if (i % 10 == 9) printf("\n");
			}
			printf("\n===========================================\n");
		}
		i++;
	}
}