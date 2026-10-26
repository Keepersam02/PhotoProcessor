

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>
int test_1(int i) {
  for (int k = 0; k < 1000; k++) {
    i++;
  }
  return i;
}

void test_2(int &i) {
  for (int k = 0; k < 500; k++) {
    i++;
  }
  return;
}

int main() {

  pid_t pid1;
  pid_t pid2;

  int i = 0;
  test_1(i);
  pid1 = fork();

  int k = 0;
  test_2(k);
  pid2 = fork();

  int status1;
  waitpid(pid1, &status1, 0);

  int status2;
  waitpid(pid2, &status2, 0);

  if (!WIFEXITED(status1)) {
    return -2;
  }
  if (!WIFEXITED(status2)) {
    return -3;
  }
  printf("%d\n", i);
  printf("%d\n", k);
  return 0;
}
