#ifndef PACKET_ANALYSIS_H_
#define PACKTE_ANALYSIS_H_

#include "cyt_packet.pb.h"
#include "ds_server.pb.h"
#include "package_define.pb.h"
#include <iostream>

//分析gs_rs之间通信包
template<class T>
bool UnpackAndAnalysePackFromGSRS(char *buff_in,int len_in,GateRoomServerPack &g_pack,StruCytPacket &s_pack,T &pack)
{
	if(!g_pack.ParseFromArray(buff_in,len_in))
	{
		std::cout<<"Unpack GateRoomServerPack error"<<std::endl;
		return false;
	}

	if(!s_pack.ParseFromArray(g_pack.data_msg().c_str(),g_pack.data_len()))
	{
		std::cout<<"Unpack StruCytPacket error"<<std::endl;
		return false;
	}

	if(!pack.ParseFromArray(s_pack.msg_data().c_str(),s_pack.msg_len()))
	{
		std::cout<<"Unpack T_pack Error"<<std::endl;
		return false;
	}

	return true;
}

//分析两层封包，包括由客户端发往GS，RS与CS通信的包
template<class T>
bool UnpackAndAnalysePackOfOtherServer(char *buff_in,int len_in,StruCytPacket &s_pack,T &pack)
{
	if(!s_pack.ParseFromArray(buff_in,len_in))
	{
		std::cout<<"Unpack StruCytPacket error"<<std::endl;
		return false;
	}

	if(!pack.ParseFromArray(s_pack.msg_data().c_str(),s_pack.msg_len()))
	{
		std::cout<<"Unpack T_pack Error"<<std::endl;
		return false;
	}

	return true;
}


#endif
