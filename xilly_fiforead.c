/*
 * xilly_fiforead.c
 * Read data out of a xillybus FIFO and into a Matlab array
 */

#include "mex.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

uint8_t do_read(void *buffer, size_t buf_size, int read_fd) {
    size_t bytes_read = 0;
    while (bytes_read < buf_size) {
        size_t max_read = buf_size - bytes_read;
        size_t status = read(read_fd, buffer + bytes_read, max_read);
        if (status==0) {
            // EOF
            mexWarnMsgIdAndTxt("xillydrv:xilly_fiforead:eof",
                    "Hit EOF - some samples may be missing");
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
    if(nrhs != 2 && nrhs != 3) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                      "Two or three inputs required");
    }
    
    // filename
    char fname[128];
    if (mxGetString(prhs[0], fname, sizeof(fname)) != 0) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                "Couldn't get filename");
    }

    // num_values
    if (!mxIsScalar(prhs[1]) || !mxIsNumeric(prhs[1])) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                "Output array size needs to be a numeric scalar");
    }
    size_t num_values = (size_t) mxGetScalar(prhs[1]);
    
    // bytes_per_sample
    uint8_t bytes_per_sample;
    mxClassID sample_type;
    if (nrhs == 3){
        if(!mxIsScalar(prhs[2]) || !mxIsNumeric(prhs[2])) {
            mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:nrhs",
                "Sample byte width needs to be a numeric scalar");
        }
        bytes_per_sample = (uint8_t) mxGetScalar(prhs[2]);
        switch (bytes_per_sample) {
            case 1:
                sample_type = mxINT8_CLASS;
                break;
            case 2:
                sample_type = mxINT16_CLASS;
                break;
            default:
                mexErrMsgTxt("Only support byte widths of 1 or 2");
        }
    } else {
        bytes_per_sample = 1;
        sample_type = mxINT8_CLASS;
    }

    // User application is responsible for conversion to complex type
    mxArray *inData = mxCreateNumericMatrix(num_values, 1, sample_type, 0);
    if (inData == NULL) {
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:mem", 
                "Couldn't allocate Matlab array");
    }
    size_t buf_size = bytes_per_sample * num_values;
    void *buffer = mxGetData(inData);

    // This keeps the memory from being swapped out and generally 
    // prevents page faults while we do the reading
    if(mlock(buffer, buf_size)) {
        perror("mlock");
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
    if(munlock(buffer, buf_size)) {
        perror("munlock");
        close(read_fd);
        mexErrMsgIdAndTxt("xillydrv:xilly_fiforead:mem", 
                "Couldn't munlock() memory");
    }

    // Close file
    close(read_fd);

    // return
    plhs[0] = inData;
}
