#include <queue>


void radix_sort(double *array, size_t size) {

    auto *rad = new std::queue<uint64_t>[65536];

    auto *data = reinterpret_cast<uint64_t *>(array);

    uint64_t mask = 65535;


    for (size_t r = 0; r < 4; r++) {

        const uint64_t vectorizedSize = (size - 16) - ((size - 16) % 4);

#ifdef INTEL
#pragma prefetch
#pragma loop_count min(10000000), max(1000000000), avg(250000000)
#endif
        for (size_t i = 0; i < vectorizedSize; i += 4) {
            uint64_t ccont[4];
            ccont[0] = (data[i] & mask) >> (r * 16);
            ccont[1] = (data[i + 1] & mask) >> (r * 16);
            ccont[2] = (data[i + 2] & mask) >> (r * 16);
            ccont[3] = (data[i + 3] & mask) >> (r * 16);

            rad[ccont[0]].push(data[i]);
            rad[ccont[1]].push(data[i + 1]);
            rad[ccont[2]].push(data[i + 2]);
            rad[ccont[3]].push(data[i + 3]);
        }

#ifdef INTEL
#pragma prefetch data
#pragma loop_count min(0), max(50), avg(3)
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
    }

    delete[] rad;
}