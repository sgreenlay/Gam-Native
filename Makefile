
cc = toolchain\cl.bat
link = toolchain\link.bat

exe = main

src = main.cpp secondary.cpp
objs = $(src:.cpp=.ll)

all: $(exe).wasm

.cpp.ll:
  $(cc) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(exe).wasm: $(objs) 
  $(link) -o $(exe) $(objs)
