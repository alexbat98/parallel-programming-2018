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
#include "merge.h"
#include <fstream>
#include <algorithm>


#ifdef __APPLE__
#include <mach/thread_policy.h>
#include <pthread.h>
#include <mach/thread_act.h>


#endif

#ifdef __APPLE__
void set_proc_affinity(int cpu) {
    thread_affinity_policy_data_t policy_data = {cpu};
    int self = pthread_mach_thread_np(pthread_self());;
    thread_policy_set(
        static_cast<thread_act_t>(self),
        THREAD_AFFINITY_POLICY,
        (thread_policy_t) &policy_data,
        THREAD_AFFINITY_POLICY_COUNT);
}
#endif

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
    bool checkNeeded = false;

    if (argc >= 3) {
        std::string argValue = std::string(argv[2]);
        if (argValue == "true") {
            checkNeeded = true;
        }
    }

    double *data = nullptr;

    if (procId == 0) {
        data = new double[n];
        std::default_random_engine generator(static_cast<unsigned int>(time(nullptr)));
        std::uniform_real_distribution<double> distribution(1, 100000000000);

        for (size_t i = 0; i < n; i++) {
            data[i] = distribution(generator);
        }
    }

#ifdef DEBUG
    std::ofstream log;
    log.open(std::to_string(procId)+".log");
#endif

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

    std::vector<double> receiveBuffer;
    receiveBuffer.reserve(n);

    MPI_Scatterv(
        data,
        sizes,
        displacements,
        MPI_DOUBLE,
        receiveBuffer.data() + procId * partSize,
        receiveCount,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);
//  scatterEndTime = MPI_Wtime();

//  sortStartTime = MPI_Wtime();
#ifdef DEBUG
    log << "Unsorted part\n";
    for (int i = 0; i < receiveCount; i++) {
        log << receiveBuffer[procId * partSize + i];
    }
    log << "\n";
#endif
    radix_sort(receiveBuffer.data() + procId * partSize, static_cast<size_t>(receiveCount));
#ifdef DEBUG
    log << "Sorted part\n";
    for (int i = 0; i < receiveCount; i++) {
        log << receiveBuffer[procId * partSize + i] << " ";
    }
    log << "\n";
#endif

    int active = procCount;
    int stage = 2;
    int step = 1;

    std::vector<double> copyBuffer;
    copyBuffer.reserve(n);
    int hasCount = receiveCount;

    double mergeStart = MPI_Wtime();
    if (procCount > 1) {
        while (active > 1) {

            if (procId % stage == 0) {
#ifdef DEBUG
                log << "I will merge\n";
#endif
                int sender = procId + step;
#ifdef DEBUG
                log << "I will receive from " << sender << " \n";
#endif

                int recvCount = 0;
                MPI_Status status;
                MPI_Recv(&recvCount, 1, MPI_INT, sender, 110, MPI_COMM_WORLD, &status);
#ifdef DEBUG
                log << "It has " << recvCount << " elements\n";

                log << "My part before Recv\n";
                for (int i = 0; i < hasCount; i++) {
                    log << receiveBuffer[procId * partSize + i] << " ";
                }
                log << "\n";
#endif

                MPI_Recv(
                    receiveBuffer.data() + procId * partSize + hasCount,
                    recvCount,
                    MPI_DOUBLE,
                    sender,
                    100,
                    MPI_COMM_WORLD,
                    &status
                );

#ifdef DEBUG
                log << "My part after Recv\n";
                for (int i = 0; i < hasCount; i++) {
                    log << receiveBuffer[procId * partSize + i] << " ";
                }
                log << "\n";

                log << "Received part Recv\n";
                for (int i = 0; i < recvCount; i++) {
                    log << receiveBuffer[procId * partSize + hasCount + i] << " ";
                }
                log << "\n";
#endif

                merge(
                    copyBuffer,
                    receiveBuffer.data() + procId * partSize,
                    hasCount,
                    hasCount + recvCount
                );


                for (size_t k = 0; k < hasCount + receiveCount; k++) {
                    receiveBuffer[procId * partSize + k] = copyBuffer[k];
                }
                hasCount += recvCount;

#ifdef DEBUG
                log << "Merged part\n";
                for (int i = 0; i < hasCount; i++) {
                    log << receiveBuffer[procId * partSize + i] << " ";
                }
                log << "\n";
#endif
            } else {
                int receiver = procId - step;
                MPI_Send(&hasCount, 1, MPI_INT, receiver, 110, MPI_COMM_WORLD);
                MPI_Send(
                    receiveBuffer.data() + (procId * active),
                    hasCount,
                    MPI_DOUBLE,
                    receiver,
                    100,
                    MPI_COMM_WORLD);

                break;
            }
            active = active / 2;
            stage = stage * 2;
            step = step * 2;
        }
    }
    double mergeEnd = MPI_Wtime();

    endTime = MPI_Wtime();

    if (procId == 0) {
        std::cout << "Total time " << endTime - startTime << std::endl;
        std::cout << "Merge time " << mergeEnd - mergeStart << std::endl;

        if (checkNeeded) {
            bool isCorrect = true;
            for (size_t i = 1; i < n; i++) {
                if (receiveBuffer[i - 1] > receiveBuffer[i]) {
                    isCorrect = false;
                    break;
                }
            }

            if (isCorrect) {
                std::cout << ".\n";
            }
#ifdef DEBUG
            else {
                std::cout << "error\n";
                for (size_t i = 0; i < n; i++) {
                    std::cout << data[i] << " ";
                }
                std::cout << "\n";
                for (size_t i = 0; i < n; i++) {
                    std::cout << receiveBuffer[i] << " ";
                }
            }
#endif
        }

//    std::cout << "Scatter took " << scatterEndTime - scatterStartTime << std::endl;
//    std::cout << "Gather took " << gatherEndTime - gatherStartTime << std::endl;
//    std::cout << "Merge took " << mergeEndTime - mergeStartTime << std::endl;
    }

//  std::cout << "For process " << procId << " radix_sort took " << sortEndTime - sortStartTime << std::endl;
//  std::cout << "Process " << procId << " sorted " << partSize << " items." << std::endl;

    MPI_Finalize();

    return 0;
}
