#ifndef CHANNEL_H
#define CHANNEL_H

#include "Server.hpp"

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

    // 채널 주제
    std::string _topic;


    ///// 채널 모드 /////
    // 비밀번호 설정 모드
    bool _isPasswordSet;

    // 초대 전용 모드
    bool _isInviteOnly;

    // 주제 보호 모드 (op만 주제 설정 가능)
    bool _isTopicProtected;

    // 유저 수 제한 모드
    bool _isUserLimitSet;

    // 채널 비밀번호
    std::string _password;

    // 채널에 설정된 사용자 제한 수
    int _userLimit;

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

    std::string getTopic();

    std::string getPassword();

    int getUserLimit();

    bool getIsPasswordSet();

    bool getIsInviteOnly();

    bool getIsTopicProtected();

    bool getIsUserLimitSet();


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
    /// 명령어
    // 자신을 제외한 모든 클라이언트에게 메시지를 보내는 메서드
    void sendToAllClients(int fd, std::string message);

    // 채널에 있는 클라이언트들을 공백으로 구분해 출력하는 메서드
    std::string getChannelClients();


    /// 클라이언트 리스트
    // 채널에 클라이언트를 추가하는 메서드
    void addClient(int fd, Client *client);

    // 채널에서 클라이언트를 제거하는 메서드
    void removeClient(int fd);

    // 채널에 닉네임의 클라이언트가 있는지 확인하는 메서드
    bool isNickInChannel(std::string nick);

    // 채널에 fd를 가지는 클라이언트가 있는지 확인하는 메서드
    bool isFdInChannel(int fd);


    /// OP 리스트
    // OP리스트에 클라이언트 추가
    void addClientToOPList(int fd);

    // OP리스트에서 클라이언트 제거
    void removeClientFromOPList(int fd);

    // fd의 클라이언트가 op인지 확인하는 메서드
    bool isFdInOPList(int fd);

    // 닉네임의 클라이언트가 op인지 확인하는 메서드
    bool isNickInOPList(std::string nick);


    /// 초대 리스트
    // 초대 리스트에 클라이언트 추가
    void addClientToInviteList(int fd);

    // 초대 리스트에 클라이언트 제거
    void removeClientFromInviteList(int fd);

    // 채널의 초대 리스트에 fd를 가지는 클라이언트가 있는지 확인하는 메서드
    bool isFdInInviteList(int fd);

    // 채널의 초대 리스트에 닉네임의 클라이언트가 있는지 확인하는 메서드
    bool isNickInInviteList(std::string nick);

};

#endif