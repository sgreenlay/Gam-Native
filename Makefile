
cc = toolchain\cl.bat
link = toolchain\link.bat

exe = main

src = main.cpp secondary.cpp
objs = $(src:.cpp=.ll)

all: $(exe).wasm

clean:
  del *.ll *.s *.wast *.wasm

.cpp.ll:
  $(cc) -c $*.cpp

$(exe).wasm: $(objs) 
  $(link) -o $(exe) $(objs)
