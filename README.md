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
            - Constant_Acceleration_Simulation_2D.h : 등가속도 운동 시뮬레이션
            - fluid_grid_2D.h : particle과 grid system ( MAC grid ) 를 적용한 시뮬레이션
            - fluid_onlyParticle_2D.h : semi-Lagrangian만을 사용하는 only particle 시뮬레이션
            - fluid_grid_2D_multithread : ""멀티 스레드 프로그래밍이 적용된 유체 시뮬레이션""
        - src
            - main.cpp : 시뮬레이션을 실행하는 파일
            - main_threadpool.cpp : ""멀티 스레드 시뮬레이션 헤더파일 및 프로그래밍을 적용한 실행 파일""
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
- 시뮬레이션 과정 정상
