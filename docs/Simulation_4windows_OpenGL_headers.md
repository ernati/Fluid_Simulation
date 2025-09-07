# 헤더 파일 문서화

## 1. `grid.h`
### 주요 클래스: `MAC_Grid`
- **설명**: Marker-And-Cell Grid를 나타내는 클래스.
- **주요 멤버**:
  - `int gridsize`: 그리드 크기.
  - `vector<T> cell_values`: 셀 값들을 저장하는 벡터.
- **주요 메서드**:
  - `int get_VectorIndex_from_cell(Vector2D i_j)`: 셀 좌표를 벡터 인덱스로 변환.
  - `T get_cell_value(Vector2D i_j)`: 특정 셀의 값을 반환.

---

## 2. `drawgrid.h`
### 주요 함수: `make_Points_of_grids`
- **설명**: 그리드의 선분 끝점을 생성.
- **입력 파라미터**:
  - `vector<Vector2D> &grid`: 그리드 벡터.
  - `int n`: 그리드 크기.
- **리턴 값**: 없음.
- **하위 단계**:
  - 가로 및 세로 선분 끝점을 계산하여 `grid`에 추가.

---

## 3. `fluid_grid_2D.h`
### 주요 열거형: `CellType`
- **설명**: 셀의 유형을 나타냄.
- **값**:
  - `FLUID`: 유체.
  - `AIR`: 공기.
  - `SOLID`: 고체.

### 주요 함수: `compare`, `compare2`
- **설명**: 파티클 및 벡터를 정렬하기 위한 비교 함수.
- **입력 파라미터**:
  - `Particle2D &a, Particle2D &b` (compare).
  - `Vector2D &a, Vector2D &b` (compare2).
- **리턴 값**: 비교 결과 (bool).

---

## 4. `box.h`
### 주요 클래스: `Box`
- **설명**: 박스의 최소/최대 좌표를 저장.
- **주요 멤버**:
  - `float xmin, xmax, ymin, ymax`: 박스의 경계 좌표.
- **생성자**:
  - 기본 생성자.
  - 좌표를 입력받는 생성자.

---

## 5. `Constant_Acceleration_Simulation_2D.h`
### 주요 클래스: `Constant_Acceleration_Simulator`
- **설명**: 일정한 가속도를 가지는 시뮬레이터.
- **주요 메서드**:
  - `particle_simulation`: 파티클 시뮬레이션 실행.
  - `Update_particles_Velocity`: 파티클 속도 업데이트.

---

## 6. `gather_simulation.h`
### 주요 클래스: `GatherSimulation`
- **설명**: 파티클을 중심으로 모으는 시뮬레이터.
- **주요 멤버**:
  - `double center_radius`: 중심 반지름.
  - `Vector2D center`: 중심 좌표.
- **주요 메서드**:
  - 생성자: 기본 생성자 및 파티클 수를 입력받는 생성자.

---

## 7. `simul_sinecosine.h`
### 주요 클래스: `Simul_SineCosine`
- **설명**: 사인 및 코사인 함수를 따라 움직이는 파티클 시뮬레이터.
- **주요 멤버**:
  - `int particle_num`: 파티클 수.
  - `unique_ptr<vector<Vector2D>> sine_particles, cosine_particles`: 사인 및 코사인 파티클.
- **주요 메서드**:
  - 생성자: 기본 생성자 및 파티클 수를 입력받는 생성자.

---

## 8. `fluid_grid_2D.h`의 추가 함수

### 8.1. `init`
<입력 파라미터> :
- `int particle_number`: 파티클 수.
- `int grid_N`: 그리드 크기.

<리턴 값> : 없음

<하위 단계들>
8.1.1. 파티클과 그리드를 초기화합니다.

### 8.2. `clear_and_ReInit`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
8.2.1. 기존 데이터를 초기화하고 재설정합니다.

### 8.3. `particle_simulation`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
8.3.1. 파티클의 위치와 속도를 업데이트합니다.

---

## 9. `Constant_Acceleration_Simulation_2D.h`의 추가 함수

### 9.1. `Update_particles_Velocity`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
9.1.1. 모든 파티클의 속도를 업데이트합니다.

### 9.2. `Update_particles_Location`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
9.2.1. 모든 파티클의 위치를 업데이트합니다.

---

## 10. `gather_simulation.h`의 추가 함수

### 10.1. `Update_Acceleration`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
10.1.1. 파티클의 가속도를 업데이트합니다.

### 10.2. `Update_Velocity`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
10.2.1. 파티클의 속도를 업데이트합니다.

---

## 11. `simul_sinecosine.h`의 추가 함수

### 11.1. `calculate_sine`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
11.1.1. 사인 함수를 기반으로 파티클의 위치를 계산합니다.

### 11.2. `calculate_cosine`
<입력 파라미터> : 없음

<리턴 값> : 없음

<하위 단계들>
11.2.1. 코사인 함수를 기반으로 파티클의 위치를 계산합니다.

---

이 문서는 `/makedocs` 규칙에 따라 작성되었으며, 각 헤더 파일의 주요 내용을 체계적으로 설명합니다. 추가적인 정보가 필요하다면 코드를 참고하거나 관련 문서를 확인하세요.
