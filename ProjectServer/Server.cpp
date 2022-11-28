#pragma comment(lib, "ws2_32")
#include <iostream>
#include <winsock2.h>
#include <thread> //��Ƽ��������
using namespace std;

#define PACKET_SIZE 1024
#define MAX 2 //�ִ���밡���� Ŭ���̾�Ʈ������

WSADATA wsa;
SOCKET skt, client_sock[MAX];
SOCKADDR_IN client[MAX] = { 0 };
int client_size[MAX];

void recvclient(SOCKET& s, int client_num) { //Ŭ���̾�Ʈ recv�Լ����� ��Ƽ������
	char buf[PACKET_SIZE];

	while (1) {
		ZeroMemory(buf, PACKET_SIZE);
		if (recv(s, buf, PACKET_SIZE, 0) == -1) break; //Ŭ���̾�Ʈ ���� ����

		cout << "\nClient #" << client_num << " << " << buf << "\n���� �����͸� �Է� >> ";
	}

	return;
}

void accpetclients() {
	char client_num[10]; //Ŭ���̾�Ʈ�������� ���ڿ��� �����ϱ����� ����� ����
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

		cout << "Client #" << i << " Joined!"<<"\n���� �����͸� �Է� >> ";; // Ŭ���̾�Ʈ ���ᰨ��
		ZeroMemory(client_num, sizeof(client_num)); // ����뺯�� �����ʱ�ȭ
		_itoa_s(i, client_num, 10); // i�� �������� client_num���ٰ� 10������ ����
		send(client_sock[i], client_num, strlen(client_num), 0); // Ŭ���̾�Ʈ��ȣ ����
		thread(recvclient, ref(client_sock[i]), i).detach(); // �ش�Ŭ���̾�Ʈ ���������
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
		cout << "���� �����͸� �Է� >> ";
		cin >> msg; //�����ͳ���

		if (!strcmp(msg, "exit")) break; // �������ǳ����� "exit"�Ͻ� ��������

		cout << "��� Ŭ���̾�Ʈ�� �Է�(all:���) >> ";
		cin >> sendnum; //���Ŭ���̾�Ʈ��ȣ���� "all"�Ͻ� ��� Ŭ���̾�Ʈ��������

		if (!strcmp(sendnum, "all")) // ����sendnum�� ������ "all" �̶�� ��ο��� �޼�������
			for (int i = 0; i < MAX; i++)
				send(client_sock[i], msg, strlen(msg), 0);
		else send(client_sock[atoi(sendnum)], msg, strlen(msg), 0); //�ƴ϶�� �Ѹ��Ը� ����
	}

	for (int i = 0; i < MAX; i++) closesocket(client_sock[i]);
	closesocket(skt);
	WSACleanup();
}