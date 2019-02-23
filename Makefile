MEX=${shell which mex}
ifeq ("$(MEX)", "")
	MEX:=${shell which matlab-mex}
endif

all: xilly_memwrite_mex.mexa64

xilly_memwrite_mex.mexa64: xilly_memwrite_mex.c
	mex $<
