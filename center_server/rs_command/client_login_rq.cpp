#include "client_login_rq.h"
#include "../../packet/package_define.pb.h"
#include "center_server.h"

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

void ClientLoginRQ::Execute(char *buff,int len,void *caller_ptr)
{
	if(!server_ptr_ || !caller_ptr)
		return;
	RoomServerProcessor *tmp_processor = reinterpret_cast<RoomServerProcessor *>(caller_ptr);
	CenterServer *tmp_server = reinterpret_cast<CenterServer *>(server_ptr_);

	ClientInfoEx *tmp_user = reinterpret_cast<ClientInfoEx *>(tmp_processor->GetParent());
	if(!tmp_user)
		return ;

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

	int tmp_verify_result = 0;
	int tmp_db_ret = tmp_server->GetMikuDB()->UserLogin(tmp_user_login.user_id(),tmp_user_login.user_psw(),tmp_verify_result);
	if(tmp_db_ret <= 0)
	{
		std::cout<<"sql execute error"<<std::endl;
		return ;
	}
	//伪验证成功
	StruUserLoginRS tmp_login_rs;
	tmp_login_rs.set_msg_id(E_USER_LOGIN_RS);
	tmp_login_rs.set_user_id(tmp_user_login.user_id());
	tmp_login_rs.set_room_id(tmp_user_login.room_id());
	tmp_login_rs.set_result(tmp_verify_result);
	
	tmp_processor->SendDataToRS(tmp_login_rs,E_USER_LOGIN_RS,tmp_user->user_sock);
}
