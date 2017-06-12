#!/usr/bin/env python

import socket
import io
import threading
import time


TCP_IP_DEFAULT = '192.168.4.1'
TCP_PORT_DEFAULT = 2000
BUFFER_SIZE = 20

is_tcp = True

print("Start TCP client")

# Get parameters

tcp_ip = input("TCP server IP (default: {}):".format(TCP_IP_DEFAULT))
if not tcp_ip:
	tcp_ip = TCP_IP_DEFAULT

tcp_port = input("TCP server port (default: {}):".format(TCP_PORT_DEFAULT))
if not tcp_port:
	tcp_port = TCP_PORT_DEFAULT
else:
	tcp_port = int(tcp_port)


connectOk = False
needRun = True
threadRecv = None
threadSend = None
s = None
tcp_send_thread_is_ok = None
tcp_received_thread_is_ok = None


def tcp_send_thread():
	global connectOk
	global needRun
	global s
	global is_tcp
	while connectOk:
		msg = input("Type your message: ")
		if msg == "Exit":
			connectOk = False
			needRun = False
		elif msg == "Disconnect":
			connectOk = False
		else:
			msg = bytes(msg.encode("ASCII"))

		try:
			if is_tcp:
				s.send(msg)
			else:
				s.sendto(msg, (tcp_ip, tcp_port))
			print("Message sent")
		except Exception as excpt:
			print("Error in sending thread" + str(excpt))
			connectOk = False
			tcp_send_thread_is_ok = False
			return
	print("Exit Send thread")
	

def tcp_receive_thread():
	global connectOk
	global needRun
	global s
	global is_tcp

	while connectOk:
		try:
			data = s.recv(BUFFER_SIZE)
			if not data:
				print("Received closing byte, pls reconnect!")
				break
			print("\nReceived data: {}".format(str(data)))
		except Exception as excpt:
			print("Error in receiving thread" + str(excpt))
			connectOk = False
			tcp_received_thread_is_ok = False
	print("Exit Receive thread")


# Main:
while needRun:
	print("Start TCP connect IP: {0} Port: {1}".format(tcp_ip, tcp_port))

	# Problem: it has large latency
	if is_tcp:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# Low latency:
	#socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
	#s = socket.socket(socket.AF_INET, socket.TCP_NODELAY)	
	else:
		# UDP?
		s = socket.socket(socket.AF_INET, # Internet
						socket.SOCK_DGRAM) # UDP
	
	connectOk = False
	try:
		if is_tcp:
			s.connect((tcp_ip, tcp_port))
		# Start threads, because ~connect is successful
		connectOk = True
		print("Successful connect")
		tcp_send_thread_is_ok = True
		tcp_received_thread_is_ok = True
		threadRecv = threading.Thread(name="Receive thread", target=tcp_receive_thread)
		threadRecv.start()
		threadSend = threading.Thread(name="Send thread", target=tcp_send_thread)
		threadSend.start()

		while connectOk:
			# If everything is ok
			time.sleep(1)
			if s._closed:
				connectOk = False
				break
	except Exception as excpt:
		print("Error in Main" + str(excpt))
		connectOk = False	
	
	connectOk = False
		
	if is_tcp:
		s.close()
