#include <unistd.h>
#include <sys/syscall.h>
 
int main() {
    syscall(SYS_write, 1, "Hello, World!\n", 14);
    return 0;
}
