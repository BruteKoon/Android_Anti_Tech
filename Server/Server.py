#-*- coding:utf-8 -*-
from socket import *

serverSock = socket(AF_INET, SOCK_STREAM)
serverSock.bind(('127.0.0.1', 9999))
serverSock.listen(1)

connectionSock, addr = serverSock.accept()

print(str(addr),'에서 접속이 확인되었습니다.')

data = connectionSock.recv(1024)
print('받은 데이터 : ', data.decode('utf-8'))

connectionSock.send('I am a server.'.encode('utf-8'))
print('메시지를 보냈습니다.')
