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

//�Լ� ������ �� �Ķ���͵��� �����ϴ� ����ü
class task {
public:
	//function<void(void*)> func_param;
	function<void()> func;
	//void *param; //parameter�� ���� ���� �𸣹Ƿ� void*�� ����

	task() {
		//func_param = nullptr;
		//param = nullptr;
		func = nullptr;
	}


	task(function<void()> f) {
		//func_param = nullptr;
		func = f;
		//param = p;
	}

	/*task(void (*f)(void*) , void* p) {
		func_param = f;
		func = nullptr;
		param = p;
	}*/
};

class simulation_task {
public:

	function<void(Fluid_Simulator_Grid&)> func;
	Fluid_Simulator_Grid* simulation;

	simulation_task() {
		func = nullptr;
	}


	simulation_task(Fluid_Simulator_Grid* simul, function<void(Fluid_Simulator_Grid&)> f) {
		func = f;
		simulation = simul;
	}

};


class thread_pool_simulation {
private:
	bool running;
	queue<task> task_queue;
	vector<thread> threads;
	int thread_num;
	mutex mtx;
	condition_variable cv;
	queue<simulation_task> simulation_task_queue;

public:
	thread_pool() {

	}

	thread_pool(int numberOfThread) {
		cout << endl << "threadpool creating start" << endl;

		this->thread_num = numberOfThread;

		this->running = true;

		//vector�� reserve
		threads.reserve(thread_num);

		//Thread ����
		for (int i = 0; i < thread_num; i++) {
			/*std::function<void(thread_pool*)> func = worker;*/
			//threads->emplace_back( &thread_pool::worker );
			threads.emplace_back([this]() { this->worker(); });
		}

		cout << endl << "threadpool creating end" << endl;

	}

	//thread_pool(const thread_pool&) = delete;

	//// ���� �Ҵ� ������ ����
	//thread_pool& operator=(const thread_pool&) = delete;

	~thread_pool() {
		//delete threads;
		running = false;
		cv.notify_all();

		for (int i = 0; i < thread_num; i++) {
			threads[i].join();
		}
	}

	//thread�鿡�� �ο��� �Լ� - thread�� �Ҵ���� task�� ����

	//void thread_pool_submit_param(void (*f)(void* p), void* p) {
	//	//mutex�� ȹ���Ѵ�.
	//	mtx.lock();

	//	//critical section
	//	task* tsk = new task(f, p);
	//	
	//	//queue�� �Լ��� �ִ´�.
	//	task_queue.push(tsk);

	//	//�ٸ� ��������� �۾��� �Ҵ���� �� �ֵ��� notify
	//	cv.notify_all();

	//	//mutex�� ��ȯ�Ѵ�.
	//	mtx.unlock();
	//}

	void Run(vector< function<void()> >* functions) {
		if (functions->size() == 0) {
			cout << endl << "error : no FUNCTIONS!!!" << endl;
		}

		for (int i = 0; i < functions->size(); i++) {
			thread_pool_submit_void((*functions)[i]);
		}
	}

	void worker() {

		cout << endl << "thread is on moving" << endl;

		task tsk;
		simulation_task sim_tsk;

		//thread_pool�� ���ư��� ���� ��
		while (running) {

			/*mtx.lock();*/
			//�׳� ���׽��� ��� unique_lock ���
			unique_lock<mutex> lock(mtx);

			//wait�� Ư���� unique lock�� ����� �� �ۿ� ����.
			//cv.wait(lock, [this]() { return !this->task_queue.empty() || !(running); } );
			cv.wait(lock, [this]() { return !this->task_queue.empty() || !this->simulation_task_queue.empty() || !(running); });

			if (!running && task_queue.empty() && simulation_task_queue.empty()) {
				// ������ Ǯ ���� ����
				break;
			}

			if (!task_queue.empty()) {
				tsk = task(task_queue.front().func);

				//queue���� �۾��� ���������Ƿ� pop
				task_queue.pop();

				//�ٸ� ��������� �۾��� �Ҵ���� �� �ֵ��� notify
				cv.notify_one();

				//critical section ����
				lock.unlock();


				tsk.func();
			}

			if (!simulation_task_queue.empty()) {
				sim_tsk = simulation_task(simulation_task_queue.front().simulation, simulation_task_queue.front().func);

				//queue���� �۾��� ���������Ƿ� pop
				simulation_task_queue.pop();

				//�ٸ� ��������� �۾��� �Ҵ���� �� �ֵ��� notify
				cv.notify_one();

				//critical section ����
				lock.unlock();


				sim_tsk.func(*sim_tsk.simulation);
			}

			else {
				cout << endl << "queue is empty!" << endl;

				lock.unlock();
			}

		}
		//pool�� ����Ǿ��ٸ�
		cout << "thread is exiting" << endl;
		return;
	};

	void thread_pool_submit_void(std::function<void()> f) {
		if (running) {

			//mutex�� ȹ���Ѵ�.
			mtx.lock();

			cout << endl << "submit function start" << endl;

			//critical section
			task tsk = task(f);

			//queue�� �Լ��� �ִ´�.
			task_queue.push(tsk);

			//�ٸ� ��������� �۾��� �Ҵ���� �� �ֵ��� notify
			//cv.notify_all();
			cv.notify_one();

			cout << endl << "submit function end" << endl;

			//mutex�� ��ȯ�Ѵ�.
			mtx.unlock();

		}

		else {
			cout << endl << "thread pool is not running so we can't submit this function" << endl;
		}
	}

	void thread_pool_submit_void_simulation(Fluid_Simulator_Grid* simulation, std::function<void(Fluid_Simulator_Grid&)> f) {
		if (running) {

			//mutex�� ȹ���Ѵ�.
			mtx.lock();

			cout << endl << "submit function start" << endl;

			//critical section
			simulation_task tsk = simulation_task(simulation, f);

			//queue�� �Լ��� �ִ´�.
			simulation_task_queue.push(tsk);

			//�ٸ� ��������� �۾��� �Ҵ���� �� �ֵ��� notify
			//cv.notify_all();
			cv.notify_one();

			cout << endl << "submit simulation function end" << endl;

			//mutex�� ��ȯ�Ѵ�.
			mtx.unlock();

		}

		else {
			cout << endl << "thread pool is not running so we can't submit this simulation function" << endl;
		}
	}
};