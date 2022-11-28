#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <thread> //멀티쓰레드사용
using namespace std;

#define PACKET_SIZE 1024
#define MAX 2 //최대수용가능한 클라이언트수지정

WSADATA wsa;
SOCKET skt, client_sock[MAX];
SOCKADDR_IN client[MAX] = { 0 };
int client_size[MAX];

void recvclient(SOCKET& s, int client_num) { //클라이언트 recv함수전용 멀티쓰레드
	char buf[PACKET_SIZE];

	while (1) {
		ZeroMemory(buf, PACKET_SIZE);
		if (recv(s, buf, PACKET_SIZE, 0) == -1) break; //클라이언트 종료 감지

		cout << "\nClient #" << client_num << " << " << buf << "\n보낼 데이터를 입력 >> ";
	}

	return;
}

void accpetclients() {
	char client_num[10]; //클라이언트정수값을 문자열로 저장하기위한 저장용 변수
	for (int i = 0; i < MAX; i++) {
		client_size[i] = sizeof(client[i]);
		client_sock[i] = accept(skt, (SOCKADDR*)&client[i], &client_size[i]);

		if (client_sock[i] == INVALID_SOCKET) {
			cout << "accept error";
			closesocket(client_sock[i]);
			closesocket(skt);
			WSACleanup();
			return;
		}

		cout << "Client #" << i << " Joined!"<<"\n보낼 데이터를 입력 >> ";; // 클라이언트 연결감지
		ZeroMemory(client_num, sizeof(client_num)); // 저장용변수 내용초기화
		_itoa_s(i, client_num, 10); // i의 정수값을 client_num에다가 10진수로 저장
		send(client_sock[i], client_num, strlen(client_num), 0); // 클라이언트번호 전송
		thread(recvclient, ref(client_sock[i]), i).detach(); // 해당클라이언트 쓰레드생성
	}
}

int main() {
	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		cout << "WSA error";
		return 0;
	}

	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (skt == INVALID_SOCKET) {
		cout << "socket error";
		closesocket(skt);
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4444);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(skt, (SOCKADDR*)&addr, sizeof(addr))) {
		cout << "bind error";
		closesocket(skt);
		WSACleanup();
		return 0;
	}
	if (listen(skt, SOMAXCONN)) {
		cout << "listen error";
		closesocket(skt);
		WSACleanup();
		return 0;
	}

	thread(accpetclients).detach();

	char msg[PACKET_SIZE], sendnum[PACKET_SIZE];

	while (1) {
		cout << "보낼 데이터를 입력 >> ";
		cin >> msg; //데이터내용

		if (!strcmp(msg, "exit")) break; // 데이터의내용이 "exit"일시 소켓종료

		cout << "대상 클라이언트를 입력(all:모두) >> ";
		cin >> sendnum; //대상클라이언트번호지정 "all"일시 모든 클라이언트에게전송

		if (!strcmp(sendnum, "all")) // 변수sendnum의 내용이 "all" 이라면 모두에게 메세지전송
			for (int i = 0; i < MAX; i++)
				send(client_sock[i], msg, strlen(msg), 0);
		else send(client_sock[atoi(sendnum)], msg, strlen(msg), 0); //아니라면 한명에게만 전송
	}

	for (int i = 0; i < MAX; i++) closesocket(client_sock[i]);
	closesocket(skt);
	WSACleanup();
}