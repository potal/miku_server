all: mysqlcpp

clean:
	rm -f mysqlcpp *.o

mysqlcpp: mysql_conn_pool.h mysql_conn_pool.cpp test_mysql_conn.cpp

	g++ -o mysqlcpp $(mysql_config --cflags) mysql_conn_pool.h mysql_conn_pool.cpp test_mysql_conn.cpp $(mysql_config --libs) -lmysqlcppconn -lpthread

