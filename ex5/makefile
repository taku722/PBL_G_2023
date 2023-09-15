CXX = g++
CXXFLAGS = -O2
OBJS = solve.o detect_by_mysql.o
LIBS = -L/usr/lib64/mysql -lmysqlclient
INCLUDES = -I/usr/include/mysql

answer.o: $(OBJS)
	$(CXX) $(CXXFLAGS) -o answer.o $(OBJS) $(LIBS) -lpthread

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

clean:
	rm -f answer.o $(OBJS)
