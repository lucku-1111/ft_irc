#ifndef CHANNEL_H
#define CHANNEL_H

#include "Server.hpp"
#include "Client.hpp"
#include <map>
#include <vector>
#include <algorithm>

class Client;

class Channel {
private:
	///// 채널 데이터 /////
	// 채널 이름
	std::string _channelName;

	// 채널에 들어와있는 클라이언트들을 저장하는 맵 : 클라이언트의 fd를 키로 사용, 클라이언트 객체 포인터를 값으로 사용
	std::map<int, Client *> _clients;

	// 채널에 초대된 클라이언트 fd들을 저장하는 벡터
	std::vector<int> _inviteList;

	// 채널에 오퍼레이터 권한을 가진 클라이언트 fd들을 저장하는 벡터
	std::vector<int> _operatorList;

	// 채널 비밀번호
	std::string _password;

	// 채널 주제
	std::string _topic;

	// 채널에 설정된 사용자 제한 수
	int _userLimit;

	///// 채널 모드 /////
	// 비밀번호 설정 모드
	bool _isPasswordSet;

	// 초대 전용 모드
	bool _isInviteOnly;

	// 주제 보호 모드 (op만 주제 설정 가능)
	bool _isTopicProtected;

	// 유저 수 제한 모드
	bool _isUserLimitSet;

public:
	///// Constructor & Destructor /////
	Channel();

	Channel(std::string channelName);

	virtual ~Channel();

	///// Getter /////
	std::string getChannelName();

	std::map<int, Client *> getClients();

	std::vector<int> getInviteList();

	std::vector<int> getOperatorList();

	std::string getPassword();

	std::string getTopic();

	bool getIsPasswordSet();

	bool getIsInviteOnly();

	bool getIsTopicProtected();

	bool getIsUserLimitSet();

	int getUserLimit();

	///// Setter /////
	void setChannelName(std::string channelName);

	void setPassword(std::string password);

	void setTopic(std::string topic);

	void setUserLimit(int userLimit);

	void setIsPasswordSet(bool isPasswordSet);

	void setIsInviteOnly(bool isInviteOnly);

	void setIsTopicProtected(bool isTopicProtected);

	void setIsUserLimitSet(bool isUserLimitSet);

	///// Method /////
	// 채널에 클라이언트를 추가하는 메서드
	void addClient(int fd, Client *client);

	// 채널에서 클라이언트를 제거하는 메서드
	void removeClient(int fd);

	// 채널에서 OP를 추가하는 메서드
	void addOperator(int fd);

	// 채널에 초대된 클라이언트를 추가하는 메서드
	void addInviteClient(int fd);

	// 자신을 제외한 모든 클라이언트에게 메시지를 보내는 메서드
	void sendToAllClients(int fd, std::string message);

	// 클라이언트가 op인지 확인하는 메서드
	bool isClientOP(int fd);

	// 클라이언트가 채널에 초대된 클라이언트인지 확인하는 메서드
	bool isClientInvited(int fd);

	// OP리스트에 클라이언트 추가
	void addClientToOPList(int fd);

	// OP리스트에서 클라이언트 제거
	void removeClientFromOPList(int fd);

    // 초대 리스트에 클라이언트 제거
    void removeClientFromInviteList(int fd);

    // 채널에 있는 클라이언트들을 공백으로 구분해 출력하는 메서드
    std::string getChannelClients();

};

#endif