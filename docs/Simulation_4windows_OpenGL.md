# Simulation_4windows_OpenGL 문서화

## 개요
`Simulation_4windows_OpenGL.cpp`는 OpenGL을 사용하여 다양한 시뮬레이션을 구현한 프로젝트입니다. 이 문서에서는 `/makedocs` 규칙에 따라 주요 함수와 로직을 문서화합니다.

---

## 주요 변수

- `Width`, `Height`: 윈도우 크기를 정의합니다.
- `vao`, `vbo`: OpenGL에서 사용하는 Vertex Array Object와 Vertex Buffer Object.
- `simulation`, `constant_acceleration_simulation`, `sinecosine_simulation`, `gather_simulation`: 다양한 시뮬레이션 객체를 관리.
- `fluids_points`, `constant_acceleration_points`, `cosine_points`, `sine_points`, `gather_points`: 시뮬레이션 결과를 저장하는 포인트 벡터.

---

## 주요 함수

### 1. `pushback_SimulationPoints_to_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
1.1. `fluids_points` 벡터를 초기화합니다.
1.2. `simulation` 객체의 `particles` 배열에서 각 위치를 `fluids_points`에 추가합니다.

---

### 2. `pushback_constant_acceleration_SimulationPoints_to_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
2.1. `constant_acceleration_points` 벡터를 초기화합니다.
2.2. `constant_acceleration_simulation` 객체의 `particles` 배열에서 각 위치를 `constant_acceleration_points`에 추가합니다.

---

### 3. `pushback_sinecosine_SimulationPoints_to_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
3.1. `cosine_points`와 `sine_points` 벡터를 초기화합니다.
3.2. `sinecosine_simulation` 객체의 `cosine_particles`와 `sine_particles` 배열에서 각 위치를 각각의 벡터에 추가합니다.

---

### 4. `pushback_gather_SimulationPoints_to_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
4.1. `gather_points` 벡터를 초기화합니다.
4.2. `gather_simulation` 객체의 `particles` 배열에서 각 위치를 `gather_points`에 추가합니다.

---

### 5. `pushback_color`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
5.1. `color` 벡터를 초기화합니다.
5.2. 각 시뮬레이션 모드에 따라 색상을 추가합니다.

---

### 6. `Update_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
6.1. `simulation` 객체의 `particles` 배열에서 각 위치를 `fluids_points`에 업데이트합니다.

---

### 7. `Update_constant_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
7.1. `constant_acceleration_simulation` 객체의 `particles` 배열에서 각 위치를 `constant_acceleration_points`에 업데이트합니다.

---

### 8. `Update_sinecosine_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
8.1. `sinecosine_simulation` 객체의 `cosine_particles`와 `sine_particles` 배열에서 각 위치를 각각의 벡터에 업데이트합니다.

---

### 9. `Update_gather_Points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
9.1. `gather_simulation` 객체의 `particles` 배열에서 각 위치를 `gather_points`에 업데이트합니다.

---

### 10. `pushback_Circle_points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
10.1. `fluids_points` 벡터를 초기화합니다.
10.2. 각 `simulation` 객체의 `particles` 위치를 기준으로 원형 좌표를 계산하여 `fluids_points`에 추가합니다.

---

### 11. `Update_Circle_points`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
11.1. 각 `simulation` 객체의 `particles` 위치를 기준으로 원형 좌표를 계산하여 `fluids_points`에 업데이트합니다.

---

### 12. `pushback_Circle_color`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
12.1. `color` 벡터를 초기화합니다.
12.2. 각 시뮬레이션 모드에 따라 색상을 추가합니다.

---

### 13. `init`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
13.1. 시뮬레이션에 필요한 포인트 및 색상 벡터를 초기화합니다.
13.2. 다양한 시뮬레이션 객체(`Fluid_Simulator_Grid`, `Constant_Acceleration_Simulator`, `Simul_SineCosine`)를 초기화합니다.

---

### 14. `idle`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
14.1. 현재 경과 시간을 `glutGet(GLUT_ELAPSED_TIME)`을 사용하여 가져옵니다.
14.2. 경과 시간이 특정 간격(120ms)일 때, 다음 작업을 수행합니다:
  - 14.2.1. `simulation`, `constant_acceleration_simulation`, `sinecosine_simulation`, `gather_simulation` 객체의 `particle_simulation` 메서드를 호출하여 시뮬레이션을 업데이트합니다.
  - 14.2.2. `Update_Points`, `Update_constant_Points`, `Update_sinecosine_Points`, `Update_gather_Points`를 호출하여 포인트 데이터를 갱신합니다.
  - 14.2.3. `pushback_color`를 호출하여 색상 데이터를 갱신합니다.
14.3. `glutPostRedisplay`를 호출하여 화면을 다시 그리도록 요청합니다.

---

### 15. `reshape`
<입력 파라미터> :
- `width: int` — 새로운 윈도우의 너비.
- `height: int` — 새로운 윈도우의 높이.

<리턴 값> : 없음

<하위 단계들>
15.1. OpenGL 뷰포트를 새로운 윈도우 크기에 맞게 조정합니다.
15.2. 투영 행렬을 업데이트하여 화면 비율을 유지합니다.

---

### 16. `keyboard`
<입력 파라미터> :
- `key: unsigned char` — 눌린 키 값.
- `x: int` — 키 입력 시 마우스 커서의 x 좌표.
- `y: int` — 키 입력 시 마우스 커서의 y 좌표.

<리턴 값> : 없음

<하위 단계들>
16.1. 특정 키 입력에 따라 시뮬레이션 상태를 변경하거나 종료합니다.

---

### 17. `display`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
17.1. 화면을 초기화합니다.
17.2. 시뮬레이션 데이터를 기반으로 화면에 그래픽을 렌더링합니다.

---

### 18. `Menu`
<입력 파라미터> :
- `Option: int` — 선택된 메뉴 옵션.

<리턴 값> : 없음

<하위 단계들>
18.1. 선택된 메뉴 옵션에 따라 시뮬레이션 모드를 변경합니다.

---

## 결론
이 문서는 `/makedocs` 규칙에 따라 작성되었으며, `Simulation_4windows_OpenGL.cpp`의 주요 로직과 함수들을 체계적으로 설명합니다. 추가적인 정보가 필요하다면 코드를 참고하거나 관련 문서를 확인하세요.
