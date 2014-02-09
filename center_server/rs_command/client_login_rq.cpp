#include "client_login_rq.h"
#include "../packet/package_define.pb.h"

ClientLoginRQ::ClientLoginRQ()
{

}

ClientLoginRQ::ClientLoginRQ(void *server_ptr)
{
	server_ptr_ = server_ptr;
}

ClientLoginRQ::~ClientLoginRQ()
{

}

void ClientLoginRQ::Execute(char *buff,int len,void *server_ptr)
{
	if(!server_ptr_)
		return;
	StruUserLoginRQ tmp_user_login;
	bool tmp_return = false;
	tmp_return = tmp_user_login.ParseFromArray(buff,len);
	if(!tmp_return)
		return ;
	std::cout<<tmp_user_login.msg_id()<<std::endl;
	std::cout<<tmp_user_login.user_id()<<std::endl;
	std::cout<<tmp_user_login.room_id()<<std::endl;
	std::cout<<tmp_user_login.user_psw()<<std::endl;
	std::cout<<tmp_user_login.user_account_name()<<std::endl;
}
