#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif // _WIN32

using namespace std;

void sleep_millisecond(int milliseconds);

void sleep_millisecond(int milliseconds) // Cross-platform sleep function
{
#ifdef _WIN32
  Sleep(milliseconds);
#else
  usleep(milliseconds * 1000);
#endif // _WIN32
}
