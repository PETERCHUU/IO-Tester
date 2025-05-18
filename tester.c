#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>

// the counter is use to find the best page size for SQL use (normally is 4096)

uint64_t IOTimeCount(int fd, int SampleCount, int readLimited)
{
      LARGE_INTEGER freq, start, end;
      QueryPerformanceFrequency(&freq);
      uint64_t counter = 0;
      char *buffer = (char *)malloc(readLimited);

      for (int i = 0; i < SampleCount; i++)
      {

            QueryPerformanceCounter(&start);
            lseek(fd, 0, SEEK_SET); // Reset file position to the start
            int bytesRead = read(fd, buffer, readLimited - 1);

            QueryPerformanceCounter(&end);
            counter += (uint64_t)(end.QuadPart - start.QuadPart) * 1e7 / freq.QuadPart;
      }
      free(buffer);
      return counter / SampleCount;
}
#else
uint64_t IOTimeCount(int fd, int SampleCount, int readLimited)
{

      struct timespec start, end;
      clock_gettime(CLOCK_REALTIME, &start);

      uint64_t counter = 0;
      char *buffer = (char *)malloc(readLimited);

      for (int i = 0; i < SampleCount; i++)
      {
            clock_gettime(CLOCK_REALTIME, &start);
            lseek(fd, 0, SEEK_SET); // Reset file position to the start
            int bytesRead = read(fd, buffer, readLimited - 1);

            clock_gettime(CLOCK_REALTIME, &end);
            uint64_t timer = (end.tv_sec * 1000000000LL + end.tv_nsec) - (start.tv_sec * 1000000000LL + start.tv_nsec);
            printf("Seconds: %ld\nNanoseconds: %ld\n", ts.tv_sec, ts.tv_nsec);
            counter += ts.tv_sec * 1000000000LL + ts.tv_nsec;
      }
      free(buffer);
      return counter / SampleCount;
}
#endif

int main(int argc, char **argv)
{
      int fd = open("tester.txt", O_RDONLY);
      if (fd == -1)
      {
            perror("Error opening file");
            return 1;
      }

      int SampleCount = 1000;
      int readLimited = 1 << 2;
      uint64_t IOTime = IOTimeCount(fd, SampleCount, readLimited);

      while (TRUE)
      {
            readLimited <<= 1;
            uint64_t newTime = IOTimeCount(fd, SampleCount, readLimited);

            if ((newTime * 1.0 / IOTime) > 1.8)
            {
                  break;
            }

            // Code to measure
            // uint64_t time = _rdtsc(); //cpu Cycles read
            IOTime = newTime;
      }

      printf("Elapsed time: %d00 ns\n", IOTime);
      printf("best page size: %d \n", readLimited >> 1);

      close(fd);

      return 0;
}
