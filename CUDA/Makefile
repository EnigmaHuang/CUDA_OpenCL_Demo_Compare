EXENAME = cuda_vec_add
NVCC    = nvcc
NVFLAGS = -O3 -Wno-deprecated-gpu-targets

CUSRCS  = $(wildcard *.cu)
OBJS    = $(CUSRCS:.cu=.o)

all: $(EXENAME)

$(EXENAME):$(OBJS)
	$(NVCC) $(NVFLAGS) $(OBJS) -o $(EXENAME) 

%.o: %.cu 
	$(NVCC) $(NVFLAGS) -c $^ 

clean:
	$(RM) *.o $(EXENAME)