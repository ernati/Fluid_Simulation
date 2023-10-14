# Fluid_Simulation


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
            - ""main.cpp : 시뮬레이션을 실행하는 파일""
            - ""main_4window.cpp : 4개의 시뮬레이션을 실행하는 소스 파일""
	        - ""main_4window_multithread.cpp : 4개의 시뮬레이션을 멀티스레드로 실행하는 소스파일""
            - main_threadpool.cpp : 멀티 스레드 시뮬레이션 헤더파일 및 프로그래밍을 적용한 실행 파일
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



## 실행파일 관련 정보
- main.cpp ~ fluid_grid_2D.h : 유체 시뮬레이션 완성본 입니다.
- main_threadpool.cpp ~ fluid_grid_2D_multithread.h : 완성본을 멀티스레드 프로그래밍으로 구현한 실행파일입니다.
- 시작 직후 오류가 발생할 수 있습니다. 다시 실행해주시면 감사드리겠습니다.


## 데이터 구조
- 시뮬레이션 과정 정리
    - 1. collision detection
        1. collision_check ( bool vector ) 선언 및 초기화
            - 단순 반복문 - 병렬가능
        2. 입자들의 속도를 이용해서 예상 위치 계산
            1. 단순 반복문 - 병렬가능
        - 1과 2 전체가 병렬가능
        1. 예상 위치 sort
            1. 병렬 불가
        2. 그것을 이용해서 particle들을 구분 및 충돌 가능 여부 판정
            1. 병렬 불가
        3. 충돌 가능한 입자들의 속도 update
            1. 병렬 가능
        
    - 2. boundary_particle
        - 병렬 가능

- 3. advection
    - 병렬 가능

1. transfer_velocity_to_grid_from_particle
    1. cell_particle_number, previous_velocity_grid 초기화
        1. 병렬가능
        2. 이전 단계와 병렬 불가
    2. particle들이 속한 셀에 particle의 정보들 합하기
        1. 병렬가능
        2. 이전 단계와 병렬 불가능
    3. cell에 포함된 particle의 속도 평균내기
        1. 병렬가능
        2. 이전단계와 병렬 불가능
    4. cell에 포함된 속도로 velocity_difference_grid 값 채우기
        1. 병렬 가능
        2. 이전 단계와 병렬 불가

1. classify_cell_type()
    1. 병렬가능

1. add_body_force()
    1. 병렬가능
    2. 5단계와 병렬가능

1. Adjust_velocity_from_bodyforce()
    1. 병렬가능
2. extrapolate_velocity_to_air_cell
    1. 병렬가능
    2. 이전 단계와 병렬 불가
3. pressure_solve
    1. matrix A,b
        1. 병렬가능
    2. solve Ax=b
        1. 병렬불가
    3. velocity update
        1. 병렬가능
4. boundarycondition_grid
    1. 병렬가능
    2. 이전단계와 병렬 불가
5. transfer_Velocity_to_particle_from_grid()
    1. 병렬가능
    2. 이전단계와 병렬 불가
6. swap_buffer()
    1. 메인스레드에서 처리 
7. rendering_fluid()
    1. 병렬가능
    2. 이전단계와 병렬 가능



## 개발시 주요 사항
SparseMatrix가 포함된 연산에서

- 자동 메모리 관리하는 함수에서 free할 때 오류가 난다
    - → free를 했는데 누군가 참조한다
    - 즉, 멀티스레드로 해버리면, 한 스레드는 아직 수행 중인데 다른 스레드에서 free해버릴 수 있다
    - → 실행 결과가 어떻게 될지 모른다.

즉 외부라이브러리를 사용한 함수를 멀티스레드에 넣을 때, 예상 못한 수많은 오류가 발생할 수 있다.



## 실행 영상 - 4 windows
<p align="center">
<img src="https://github.com/ernati/Fluid_Simulation/assets/31719912/d4c458de-10e7-4b3e-ba8c-2b78a01657ac">
</p>
