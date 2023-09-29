#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

#include "../2D/simulation/fluid_grid_2D.h"

#define POOL_FAIL 0
#define POOL_SUCCESS 1

using namespace std;

//함수 포인터 및 파라미터들을 저장하는 구조체
class task {
public:
	//function<void(void*)> func_param;
	function<void()> func;
	//void *param; //parameter로 뭐가 올지 모르므로 void*로 선언

	task() {
		//func_param = nullptr;
		//param = nullptr;
		func = nullptr;
	}


	task(function<void()> f) {
		//func_param = nullptr;
		func = f ;
		//param = p;
	}

	/*task(void (*f)(void*) , void* p) {
		func_param = f;
		func = nullptr;
		param = p;
	}*/
};


class thread_pool {
private:
	bool running;
	queue<task> task_queue;
	vector<thread> threads;
	int thread_num;
	mutex mtx;
	condition_variable cv;

public:
	thread_pool() {

	}

	thread_pool(int numberOfThread) {
		cout << endl << "threadpool creating start" << endl;

		this->thread_num = numberOfThread;

		this->running = true;

		//vector에 reserve
		threads.reserve(thread_num);

		//Thread 생성
		for (int i = 0; i < thread_num; i++) {
			/*std::function<void(thread_pool*)> func = worker;*/
			//threads->emplace_back( &thread_pool::worker );
			threads.emplace_back([this]() { this->worker(); });
		}

		cout << endl << "threadpool creating end" << endl;

	}

	//thread_pool(const thread_pool&) = delete;

	//// 복사 할당 연산자 삭제
	//thread_pool& operator=(const thread_pool&) = delete;

	~thread_pool() {
		//delete threads;
		running = false;
		cv.notify_all();

		for (int i = 0; i < thread_num; i++) {
			threads[i].join();
		}
	}

	//thread들에게 부여될 함수 - thread가 할당받은 task를 수행

	//void thread_pool_submit_param(void (*f)(void* p), void* p) {
	//	//mutex를 획득한다.
	//	mtx.lock();

	//	//critical section
	//	task* tsk = new task(f, p);
	//	
	//	//queue에 함수를 넣는다.
	//	task_queue.push(tsk);

	//	//다른 스레드들이 작업을 할당받을 수 있도록 notify
	//	cv.notify_all();

	//	//mutex를 반환한다.
	//	mtx.unlock();
	//}

	void Run( vector< function<void()> >* functions ) {
		if (functions->size() == 0) {
			cout << endl << "error : no FUNCTIONS!!!" << endl;
		}

		for (int i = 0; i < functions->size(); i++) {
			thread_pool_submit_void( (*functions)[i] );
		}
	}

	void worker() {

		cout << endl << "thread is on moving" << endl;

		task tsk;

		//thread_pool이 돌아가고 있을 때
		while (running) {

			/*mtx.lock();*/
			//그냥 뮤테스락 대신 unique_lock 사용
			unique_lock<mutex> lock(mtx);

			//wait은 특성상 unique lock을 사용할 수 밖에 없다.
			//cv.wait(lock, [this]() { return !this->task_queue.empty() || !(running); } );
			cv.wait(lock, [this]() { return !this->task_queue.empty() || !(running); });

			if (!running && task_queue.empty() ) {
				// 스레드 풀 종료 조건
				break;
			}

			if (!task_queue.empty() ) {
				tsk = task(task_queue.front().func);

				//queue에서 작업을 가져왔으므로 pop
				task_queue.pop();

				//다른 스레드들이 작업을 할당받을 수 있도록 notify
				cv.notify_one();

				//critical section 종료
				lock.unlock();


				tsk.func();
			}

			else {
				cout << endl << "queue is empty!" << endl;

				lock.unlock();
			}

		}
		//pool이 종료되었다면
		cout << "thread is exiting" << endl;
		return;
	};

	void thread_pool_submit_void(std::function<void()> f) {
		if (running) {

			//mutex를 획득한다.
			mtx.lock();

			cout << endl << "submit function start" << endl;

			//critical section
			task tsk = task(f);

			//queue에 함수를 넣는다.
			task_queue.push(tsk);

			//다른 스레드들이 작업을 할당받을 수 있도록 notify
			//cv.notify_all();
			cv.notify_one(); 

			cout << endl << "submit function end" << endl;

			//mutex를 반환한다.
			mtx.unlock();

		}
		
		else {
			cout << endl << "thread pool is not running so we can't submit this function" << endl;
		}
	}

};