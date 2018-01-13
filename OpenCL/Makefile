EXENAME = ocl_vec_add
NVCC    = nvcc
NVFLAGS = -O3 -lOpenCL -Wno-deprecated-gpu-targets

CPPSRCS = $(wildcard *.cpp)
OBJS    = $(CPPSRCS:.cpp=.o)

all: $(EXENAME)

$(EXENAME): $(OBJS)
	$(NVCC) $(NVFLAGS) $(OBJS) -o $(EXENAME) 

%.o : %.cpp
	$(NVCC) $(NVFLAGS) -c $^ 

clean:
	$(RM) *.o $(EXENAME)