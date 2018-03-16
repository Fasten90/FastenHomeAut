#!/usr/bin/env python

import socket


TCP_IP = '192.168.1.62'
TCP_PORT = 2000
BUFFER_SIZE = 20  # Normally 1024, but we want fast response

print ("Start TCP Server, IP: {0} Port: {1}".format(TCP_IP, TCP_PORT))

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))


def tcp_server_thread():
	while 1:
		s.listen(1)
	
		conn, addr = s.accept()
		print ("Connection address:", addr)
		while 1:
			data = conn.recv(BUFFER_SIZE)
			if data:
				print ("Received data: ", str(data))
			else:
				print ("Not received data")
			#conn.send(data)  # echo
		conn.close()

