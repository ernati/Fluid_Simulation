# Fluid_Simulation

## 사용하는 방법
1. 프로젝트 clone
2. FluidSimulation/projects/ParticleSimulation/ParticleSimulation.sln 파일 실행
3. visual studio를 통해 실행 및 빌드하시면 됩니다.
   - 하단 4 windows 프로젝트는 "Simulation_4windows_OpenGL" 입니다. 해당 프로젝트를 시작 프로젝트로 설정 후, 컴파일 및 실행, 빌드 하시면 됩니다.
   - 다른 프로젝트도 모두 실행 및 빌드 가능한 상태로 repo 구성하였습니다.

## 구현을 위한 학습 내용
1. [논문1 - Animating Explosions - Gary D Yngve, James F O'Bri 1fe97d11e62d48ba988bb5172cf3b189.pdf](https://github.com/ernati/Fluid_Simulation/files/13201146/1.-.Animating.Explosions.-.Gary.D.Yngve.James.F.O.Bri.1fe97d11e62d48ba988bb5172cf3b189.pdf)

2. [논문2 - A material point method for snow simulation - 2013 fc4b233625c94613a1a7ca14488361d8.pdf](https://github.com/ernati/Fluid_Simulation/files/13201147/2.-.A.material.point.method.for.snow.simulation.-.2013.fc4b233625c94613a1a7ca14488361d8.pdf)

3. [학습교재 1 - Fluid Simulation.pdf](https://github.com/ernati/Fluid_Simulation/files/13201149/1.-.Fluid.Simulation.pdf)

### 각 논문 간단 설명
논문1 : 폭발을 표현하기 위한 물리적 요소 및 구현 모델

논문2 : MPM - 고체와 비슷한 성질을 가지는 유체인 눈을 표현하기 위한 새로운 기법 - particle 기법과 grid 기법의 혼합

학습교재 1 - 유체를 시뮬레이션 하기 위한 구현 방법 및 다양한 유체 구현 소개 교재

## 유체 시뮬레이션의 목적
입자들을 유체( 여기서는 물입니다.)처럼 움직이게 하기 위해 물리 공식들을 이산화 및 적용해서 구현하는 과정입니다.
particle에서 계산 후, 연산량 및 성능을 위해 grid로 옮겨서 계산하는 과정을 반복합니다.

## 유체 시뮬레이션 순서
1. collision detection - 입자간의 충돌 감지 및 충돌 처리 - 운동량 보존법칙
2. boundary_particle - 입자 속도 계산 때, boundary를 넘어가지 않게 하는 경계값처리
3. advection - 입자의 속도를 통한 위치 update - forward euler 적용
4. transfer_velocity_to_to_grid_from_particle - 입자의 속도를 입자가 속한 cell들로 옮김. 즉 particle의 정보를 grid로 옮김
5. classify_cell_type : particle이 속한 cell을 FLUID, 시뮬레이션 boundary를 SOLID, 나머지 cell을 AIR로 분류
6. add_body_force : 셀에 가속도(중력) 추가
7. Adjust_velocity_from_bodyforce : 가속도를 통한 속도값 업데이트 - forward euler 적용
8. extrapolate_velocity_to_air_cell : FLUID cell의 주변 AIR들에 속도값 전이
9. pressure_solve : 시뮬레이션의 메인. 각 cell들의 압력 계산 및 압력을 통한 새로운 속도 계산.
10. boundarycondition_grid : 각 셀들의 속도 계산 때, boundary를 넘어가지 않게 하는 경계값 처리
11. transfer_Velocity_to_particle_from_grid : cell의 속도를, 그 셀에 있는 particle들에 전달
12. swap_buffer : 속도 값 버퍼의 포인터를 서로 맞바꿈
13. rendering_fluid : FLUID cell의 중심점을 vector에 담음


## 실행 영상 - 4 windows
- 몇초정도 기다리시면 나옵니다!
- 왼쪽 아래가 Fluid Simulation입니다.

<p align="center">
<img src="https://github.com/ernati/Fluid_Simulation/assets/31719912/d4c458de-10e7-4b3e-ba8c-2b78a01657ac">
</p>


### 다른 포트폴리오들 
- 게임 분야라면 게임 포트폴리오, 그래픽스 분야라면 객체 지향을 경험한 포트폴리오가 되겠습니다.

게임 포트폴리오(ppt) 구글드라이브 주소 : https://docs.google.com/presentation/d/1c2pjuWhBw2z4wi7SkzCHnZoQi1-Y7UvT/edit?usp=drive_link&ouid=103736708326537874746&rtpof=true&sd=true
게임 포트폴리어(pdf) 구글드라이브 주소 : https://drive.google.com/file/d/1Hs2oozFPPrbR6G6aC5oQxnDZpBKaIe5e/view?usp=drive_link
타워 디펜스 + 1인칭 FPS 게임: https://github.com/ernati/Defense_3D
