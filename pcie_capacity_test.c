#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [devfile]\n", argv[0]);
        return -1;
    }

    int fh=open(argv[1], O_WRONLY);

    unsigned char buf[2048];

    for(int i=0; i<5; i++) {
        int total=0;

        clock_t end_time = clock() + 1*CLOCKS_PER_SEC;

        while(clock() < end_time) {
            int written = write(fh, buf, 1024);
            if (written > 0){
                total+= written;
            }
        }

        printf("%i\n",total);
    }
    return 0;
}
