### 다른 포트폴리오들 
- 게임 분야라면 게임 포트폴리오, 그래픽스 분야라면 객체 지향을 경험한 포트폴리오가 되겠습니다.

게임 포트폴리오 구글드라이브 주소 : https://docs.google.com/presentation/d/1vixeQgtY4vgsDwSxcFdS2A0bMFtm7bYl/edit?usp=drive_link&ouid=103736708326537874746&rtpof=true&sd=true
타워 디펜스 + 1인칭 FPS 게임: https://github.com/ernati/Defense_3D


# Fluid_Simulation

## 구현을 위한 학습 내용
1. [논문1 - Animating Explosions - Gary D Yngve, James F O'Bri 1fe97d11e62d48ba988bb5172cf3b189.pdf](https://github.com/ernati/Fluid_Simulation/files/13201146/1.-.Animating.Explosions.-.Gary.D.Yngve.James.F.O.Bri.1fe97d11e62d48ba988bb5172cf3b189.pdf)

2. [논문2 - A material point method for snow simulation - 2013 fc4b233625c94613a1a7ca14488361d8.pdf](https://github.com/ernati/Fluid_Simulation/files/13201147/2.-.A.material.point.method.for.snow.simulation.-.2013.fc4b233625c94613a1a7ca14488361d8.pdf)

3. [학습교재 1 - Fluid Simulation.pdf](https://github.com/ernati/Fluid_Simulation/files/13201149/1.-.Fluid.Simulation.pdf)

### 각 논문 간단 설명
논문1 : 폭발을 표현하기 위한 물리적 요소 및 구현 모델

논문2 : MPM - 고체와 비슷한 성질을 가지는 유체인 눈을 표현하기 위한 새로운 기법 - particle 기법과 grid 기법의 혼합

학습교재 1 - 유체를 시뮬레이션 하기 위한 구현 방법 및 다양한 유체 구현 소개 교재

## 폴더 구조
- simulation
    - 2D
        - header_background : 시뮬레이션에 필요한 기본 헤더파일들이 들어갈 폴더
            - box.h : 시뮬레이션이 실행된 테두리 박스 관련 헤더파일
            - drawgrid.h : grid를 opengl에 그리는 헤더파일
            - grid.h : grid system 구현 내용 관련 헤더파일
            - myVector2D.h : 연산에 사용할 2D vector 클래스 구현
            - particle.h : 시뮬레이션에 사용될 입자 클래스 구현
            - quadratic_particle.h : 예전에 사용한 연습용 헤더파일. 보지 않으셔도 됩니다.
        - practiceThings : 과거 연습한 내용들이 포함됨. 보지 않으셔도 됩니다.
            - 2D_particle_simulation.cpp
            - 2D_particle_simulation.h
        - Simulation : 다양한 시뮬레이션들 구현해 놓은 폴더
            - Constant_Acceleration_Simulation_2D.h : 등가속도 운동 시뮬레이션 헤더파일
	        - gather_simulation.h : 입자가 원형 파동처럼 중심을 향해 가는 시뮬레이션 헤더파일
            - simul_sinecosine.h : 입자가 사인함수와 코사인함수 형태로 움직이는 시뮬레이션 헤더파일
            - fluid_grid_2D.h : particle과 grid system ( MAC grid ) 를 적용한 시뮬레이션 헤더파일
            - fluid_onlyParticle_2D.h : semi-Lagrangian만을 사용하는 only particle 시뮬레이션 헤더파일
            - fluid_grid_2D_multithread : 멀티 스레드 프로그래밍이 적용된 유체 시뮬레이션     
        - src
            - main.cpp : 시뮬레이션을 실행하는 파일
            - main_4window.cpp : 4개의 시뮬레이션을 실행하는 소스 파일
	    - main_4window_multithread.cpp : 4개의 시뮬레이션을 멀티스레드로 실행하는 소스파일
            - main_threadpool.cpp : 구버전 실행파일 - 미사용
            - fshader_2dBezier.glsl : opengl 용 fshader
            - fshader_2dBezier_test.glsl : ""
            - vshader_2dBezier.glsl : opengl 용 vshader
            - vshader_2dBezier_test.glsl : ""
            - quadratic_particle_simulation.cpp : opengl 용 fshader
    - 3D : 2D에서 3D로 옮겨오는 버전.
        - header_background
            - box.h
            - box3D.h
            - drawgrid3D.h
            - grid3D.h
            - myVector3D.h
            - particle3D.h
        - Simulation
            - fluid_grid_3D.h
        - src
            - main_3D.cpp
            - fshader_2dBezier.glsl
            - vshader_2dBezier.glsl

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
<p align="center">
<img src="https://github.com/ernati/Fluid_Simulation/assets/31719912/d4c458de-10e7-4b3e-ba8c-2b78a01657ac">
</p>
