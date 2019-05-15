/*
 * xilly_fiforead.c
 * Read data out of a xillybus FIFO and into a Matlab array
 */

#include "mex.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

uint8_t do_read(void *buffer, size_t buf_size, int read_fd) {
    size_t bytes_read = 0;
    while (bytes_read < buf_size) {
        size_t max_read = buf_size - bytes_read;
        size_t status = read(read_fd, buffer, max_read);
        if (status==0) {
            // EOF
            return 0; // TODO will leave bunches of zeros where data should be
        }
        if (status < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                status = 0;
            } else {
                perror("do_read");
                return errno;
            }
        }
        bytes_read += status;
    }
    return 0;
}

/**
 * xilly_fiforead(fname, num_values, [type?])
 * Read num_values numbers out of the FIFO and into memory.
 * For now, we assume int16 type, might make this configurable eventually
 * Note that we mlock() the entire memory space for latency reasons, so 
 * make sure that your computer has enough memory to hold the entire data set
 */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    if(nrhs != 2) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                      "Two inputs required");
    }
    
    // filename
    char fname[128];
    if (mxGetString(prhs[0], fname, sizeof(fname)) != 0) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                "Couldn't get filename");
    }

    // num_values
    if(!mxIsScalar(prhs[1]) || !mxIsNumeric(prhs[1])) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                "Output array size needs to be a numeric scalar");
    }
    size_t num_values = (size_t) mxGetScalar(prhs[1]);

    // User application is responsible for conversion to complex type
    mxArray *inData = mxCreateNumericMatrix(num_values, 1, mxINT16_CLASS, 0);
    if (inData == NULL) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:mem", 
                "Couldn't allocate Matlab array");
    }
    size_t numSize = 2; // int16
    size_t buf_size = numSize * num_values;
    void *buffer = mxGetData(inData);

    // This keeps the memory from being swapped out and generally 
    // prevents page faults while we do the reading
    if(mlock(buffer, buf_size)) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:mem", 
                "Couldn't mlock() memory");
    }

    // Open the Xillybus device file
    int read_fd = open(fname, O_RDONLY);

    // Do the reads
    uint8_t status = do_read(buffer, buf_size, read_fd);
    if(status != 0) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:read",
                "Error while copying data from device file");
    }

    // Unlock memory to make sure we don't run out of RAM
    if(munlock(inData, buf_size)) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:mem", 
                "Couldn't munlock() memory");
    }

    // return
    plhs[0] = inData;
}
