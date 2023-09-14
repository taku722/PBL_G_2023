CXX = g++
CXXFLAGS = -O2
OBJS = attack.o detect_by_mysql.o
LIBS = -L/usr/lib64/mysql -lmysqlclient
INCLUDES = -I/usr/include/mysql

rho.o: $(OBJS)
	$(CXX) $(CXXFLAGS) -o rho.o $(OBJS) $(LIBS) -lpthread

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

clean:
	rm -f rho.o $(OBJS)
