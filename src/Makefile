AR       := ar
ARFLAGS  := cr
CXX      := g++
CPPFLAGS := -O2 -I../lib

all : ../bin/DPT_balance_color
	@echo -n ""

clean :
	rm *.o ../bin/DPT_balance_color

../bin/DPT_balance_color : main.o graph.o sys_cmd.o GDalgorithm.o shape.o ../lib/libgraphlab.a
	$(CXX) $^ -o $@

main.o : main.cpp graph.h sys_cmd.h user_cmd.h shape.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

graph.o : graph.cpp graph.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

sys_cmd.o : sys_cmd.cpp sys_cmd.h ../lib/cmd.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

GDalgorithm.o : GDalgorithm.cpp user_cmd.h ../lib/cmd.h
	$(CXX) $(CPPFLAGS) -c $< -o $@
	
shape.o : shape.cpp shape.h ../lib/cmd.h
	$(CXX) $(CPPFLAGS) -c $< -o $@


