#include <iostream>
#include <cmath>
#include <vector>
#include <mpi.h>
#include <random>
#include <iomanip>
#include <set>
#include <string>
#include <limits>
#include <queue>
#include "radix_sort.h"

#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <pthread.h>
#include <mach/thread_act.h>
#endif

#ifdef __APPLE__
void set_proc_affinity(int cpu) {
  thread_affinity_policy_data_t policy_data = { cpu };
  int self = pthread_mach_thread_np(pthread_self());;
  thread_policy_set(static_cast<thread_act_t>(self), THREAD_AFFINITY_POLICY, (thread_policy_t)&policy_data, THREAD_AFFINITY_POLICY_COUNT);
}
#endif

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
  auto * receiveBuffer = reinterpret_cast<double*>(_mm_malloc(receiveCount*sizeof(double), 64));
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