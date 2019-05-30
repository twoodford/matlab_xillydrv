MEX=${shell which mex}
ifeq ("$(MEX)", "")
	MEX:=${shell which matlab-mex}
endif

all: xilly_memwrite_mex.mexa64 xilly_fiforead.mexa64

xilly_memwrite_mex.mexa64: xilly_memwrite_mex.c
	$(MEX) $<

xilly_fiforead.mexa64: xilly_fiforead.c
	$(MEX) -R2018a $<

clean:
	rm -f *.mex* pcie_capacity_test
