#include "pch.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <mpi.h>
#include <random>
#include <iomanip>
#include <set>
#include <string>
#include <limits>
#include "queue.h"

#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <pthread.h>
#include <mach/thread_act.h>
#endif

#ifdef INTEL
#include <cstdlib>
#include <xmmintrin.h>
#define SIMDPP_ARCH_X86_AVX2
#include "simdpp/simd.h"
#endif

#ifdef __APPLE__
void set_proc_affinity(int cpu) {
	thread_affinity_policy_data_t policy_data = { cpu };
	int self = pthread_mach_thread_np(pthread_self());;
	thread_policy_set(static_cast<thread_act_t>(self), THREAD_AFFINITY_POLICY, (thread_policy_t)&policy_data, THREAD_AFFINITY_POLICY_COUNT);
}
#endif

void radix_sort(double *array, size_t size) {

	auto* rad = new queue<uint64_t>[65536];

	auto *data = reinterpret_cast<uint64_t*>(array);

	uint64_t mask = 65535;


	for (size_t r = 0; r < 4; r++) {

		const uint64_t vectorizedSize = (size - 16) - ((size - 16) % 4);

#ifdef INTEL
#pragma noparallel
#pragma prefetch data:_MM_HINT_T2:L2CACHE_SIZE / OPTIMAL_THREADS / 16
#pragma prefetch data:_MM_HINT_T0:L1CACHE_SIZE / OPTIMAL_THREADS / 16
#pragma loop_count min(1),max(2000000000),avg(200000000)
#endif
		for (size_t i = 0; i < vectorizedSize; i += 4) {
#ifdef INTEL
			simdpp::uint64<4> c = simdpp::load(data + i);
			uint64_t bits = r * 16;
			//simdpp::uint64<4> shift = simdpp::load_splat(&bits);
			c = c & mask;
			c = c >> bits;
			SIMDPP_ALIGN(64) uint64_t ccont[4];
			simdpp::store(ccont, c);
#else
			uint64_t ccont[4];
			ccont[0] = (data[i] & mask) >> (r * 16);
			ccont[1] = (data[i + 1] & mask) >> (r * 16);
			ccont[2] = (data[i + 2] & mask) >> (r * 16);
			ccont[3] = (data[i + 3] & mask) >> (r * 16);
#endif
			rad[ccont[0]].push(data[i]);
			rad[ccont[1]].push(data[i+1]);
			rad[ccont[2]].push(data[i+2]);
			rad[ccont[3]].push(data[i+3]);
#ifdef INTEL
			_mm_prefetch((const char *)data + i * 16 + 16, _MM_HINT_T0);
			_mm_prefetch((const char *)data + i * 16 + L2CACHE_SIZE / OPTIMAL_THREADS / 64, _MM_HINT_T1);
#endif		_mm_prefetch((const char *)data + i * 16 + L2CACHE_SIZE / OPTIMAL_THREADS / 8, _MM_HINT_T2);
		}

#ifdef INTEL
#pragma prefetch data
#pragma loop_count min(0),max(50),avg(3)
#endif
		for (size_t i = vectorizedSize; i < size; i++) {
			uint64_t c = (data[i] & mask) >> (r * 16);
			rad[c].push(data[i]);
		}

		mask = mask << 16;

		size_t idx = 0;
#ifdef INTEL
#pragma prefetch data
#pragma prefetch rad
#endif
		for (int i = 0; i < 65536; i++) {
			while (!rad[i].empty()) {
				data[idx++] = rad[i].front();
				rad[i].pop();
			}
		}
		//usedIdx.clear();
	}

	delete[] rad;
}

void merge(const double* data, const int* sizes, const int* displacements, int p, int n, double* buffer) {

	int *pointers = new int[p];

	for (int i = 0; i < p; i++) {
		pointers[i] = displacements[i];
	}

	for (int i = 0; i < n; i++) {
		double min = std::numeric_limits<double>::max();
		int idx = 0;
		for (int k = 0; k < p; k++) {
			if (min > data[pointers[k]] && pointers[k] < displacements[k] + sizes[k]) {
				idx = k;
				min = data[pointers[k]];
			}
		}
		pointers[idx]++;
		buffer[i] = min;
	}

}

int main(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);

	int procId, procCount;
	double startTime, endTime;
	//  double scatterStartTime, scatterEndTime;
	//  double gatherStartTime, gatherEndTime;
	//  double mergeStartTime, mergeEndTime;
	//  double sortStartTime, sortEndTime;

	MPI_Comm_rank(MPI_COMM_WORLD, &procId);
	MPI_Comm_size(MPI_COMM_WORLD, &procCount);

#ifdef __APPLE__
	set_proc_affinity(procId);
#endif

	const int n = std::stoi(std::string(argv[1]));

	double *data = nullptr;

	if (procId == 0) {
		data = new double[n];
		std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
		std::uniform_real_distribution<double> distribution(1, 100000000000);

		for (size_t i = 0; i < n; i++) {
			data[i] = distribution(generator);
		}
	}

	int partSize = n / procCount;
	int tail = n % procCount;

	int *sizes = procId == 0 ? new int[procCount] : nullptr;
	int *displacements = procId == 0 ? new int[procCount] : nullptr;

	if (procId == 0) {
		for (int i = 0; i < procCount - 1; i++) {
			sizes[i] = partSize;
			displacements[i] = i * partSize;
		}
		sizes[procCount - 1] = (partSize + tail);
		displacements[procCount - 1] = n - (partSize + tail);
	}

	startTime = MPI_Wtime();

	int receiveCount =
		procId == procCount - 1 ? (partSize + tail) : partSize;

#ifndef INTEL
	auto *receiveBuffer = new double[receiveCount];
#else
	auto * receiveBuffer = reinterpret_cast<double*>(_mm_malloc(receiveCount * sizeof(double), 64));
#endif

	//  scatterStartTime = MPI_Wtime();
	MPI_Scatterv(data, sizes, displacements, MPI_DOUBLE, receiveBuffer, receiveCount, MPI_DOUBLE, 0,
		MPI_COMM_WORLD);
	//  scatterEndTime = MPI_Wtime();

	//  sortStartTime = MPI_Wtime();
	radix_sort(receiveBuffer, static_cast<size_t>(receiveCount));
	//  sortEndTime = MPI_Wtime();

	//  gatherStartTime = MPI_Wtime();
	MPI_Gatherv(receiveBuffer, receiveCount, MPI_DOUBLE, data, sizes, displacements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//  gatherEndTime = MPI_Wtime();

	double* res = nullptr;

	if (procId == 0) {
		res = new double[n];

		//    mergeStartTime = MPI_Wtime();
		merge(data, sizes, displacements, procCount, n, res);
		//    mergeEndTime = MPI_Wtime();
	}

	endTime = MPI_Wtime();

	if (procId == 0) {
		std::cout << "Total time " << endTime - startTime << std::endl;
		//    std::cout << "Scatter took " << scatterEndTime - scatterStartTime << std::endl;
		//    std::cout << "Gather took " << gatherEndTime - gatherStartTime << std::endl;
		//    std::cout << "Merge took " << mergeEndTime - mergeStartTime << std::endl;
	}

	//  std::cout << "For process " << procId << " radix_sort took " << sortEndTime - sortStartTime << std::endl;
	//  std::cout << "Process " << procId << " sorted " << partSize << " items." << std::endl;

	MPI_Finalize();

	return 0;
}