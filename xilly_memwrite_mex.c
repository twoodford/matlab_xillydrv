#include "mex.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include <stdio.h>

void write_byte_value(char *fname, off_t offset, unsigned char value);

/**
 * xilly_memwrite(fname, offset, value)
 */
void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[])
{
    char *fname;
    off_t offset;
    unsigned char value;
    
    if(nrhs != 3) {
        mexErrMsgIdAndTxt("MyToolbox:arrayProduct:nrhs",
                      "Three inputs required.");
    }
    
    size_t fn_size = (mxGetM(prhs[0])*mxGetN(prhs[0]))+1;
    fname = mxCalloc(fn_size, sizeof(char));
    if (mxGetString(prhs[0], fname, fn_size) != 0) {
        mexWarnMsgTxt("GetString() failed");
    }

    // Get offset parameter
    if (mxIsClass(prhs[1], "int32") && mxGetN(prhs[1])==1 && mxGetM(prhs[1])==1) {
        offset = *((int *) mxGetData(prhs[1]));
    } else {
        mexWarnMsgTxt("offset param is wrong type");
    }

    // Get value parameter
    if (mxIsClass(prhs[2], "uint8") && mxGetN(prhs[2])==1 && mxGetM(prhs[2])==1) {
        value = *((unsigned char *) mxGetData(prhs[2]));
    } else {
        mexWarnMsgTxt("value param is wrong type");
    }

    printf("value=%u\n", value);

    write_byte_value(fname, offset, value);
}

void write_byte_value(char *fname, off_t offset, unsigned char value) {
    int fd = open(fname, O_WRONLY);
    if (fd < 0) {
        if (errno==ENODEV) {
            mexErrMsgTxt("Maybe this is a read-only file?");
        } else {
            mexErrMsgTxt("Couldn't open device file");
        }
        return;
    }

    if (lseek(fd, offset, SEEK_SET) < 0) {
        mexErrMsgTxt("Couldn't lseek()");
        return;
    }

    int wout;
    while ((wout=write(fd, &value, 1))==0) {}
    if (wout < 0){
        perror("write() failed");
        mexErrMsgTxt("write() failed");
    }
    
    close(fd);
}
