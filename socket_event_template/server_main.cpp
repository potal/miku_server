/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年01月07日 14时38分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include "test_server.h"

#include <iostream>
int main()
{
	TestServer tmp_server;
	if(tmp_server.InitServer())
		tmp_server.StartServer();
	std::cout<<"server quit"<<std::endl;
	return 0;
}
