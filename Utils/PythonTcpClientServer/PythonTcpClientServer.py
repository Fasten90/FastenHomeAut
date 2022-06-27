#!/usr/bin/env python

import socket
import io
import threading
import time


# Connection configs
# TODO: Read own IP + autodetect settings

#TCP_IP_DEFAULT = '192.168.4.1'
TCP_IP_DEFAULT = '192.168.0.241'
TCP_PORT_DEFAULT = 2000
BUFFER_SIZE = 20

is_tcp = True
is_server = True
get_parameter = False


print("Start TCP/UDP script")


# Get parameters
if get_parameter:
	tcp_ip = input("TCP server IP (default: {}):".format(TCP_IP_DEFAULT))
	if not tcp_ip:
		tcp_ip = TCP_IP_DEFAULT
	
	tcp_port = input("TCP server port (default: {}):".format(TCP_PORT_DEFAULT))
	if not tcp_port:
		tcp_port = TCP_PORT_DEFAULT
	else:
		tcp_port = int(tcp_port)
else:
	tcp_ip = TCP_IP_DEFAULT
	tcp_port = TCP_PORT_DEFAULT


# Print "TCP server starting..."
print(("TCP" if is_tcp else "UDP" )+ " " + ("server" if is_server else "client") + " starting...")


# Global variables
connectOk = False
needRun = True
threadRecv = None
threadSend = None
s = None
conn = None
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
			# TODO: Check, it is necessary?!
			msg += '\n'
			#
			msg = bytes(msg.encode("ASCII"))

		try:
			if is_tcp:
				if is_server:
					conn.send(msg)
				else:
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
	global conn
	global is_tcp

	while connectOk:
		try:
			if is_server:
				data = conn.recv(BUFFER_SIZE)
			else:
				data = s.recv(BUFFER_SIZE)
			if not data:
				print("Received closing byte, pls reconnect!")
				break
			print("Received data: {}".format(str(data)))
		except Exception as excpt:
			print("Error in receiving thread" + str(excpt))
			connectOk = False
			tcp_received_thread_is_ok = False
			break
	print("Exit Receive thread")


# Main:
while needRun:
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
		if is_server:
			# Server
			# Wait connect
			print("Start server, wait client to IP: {}:{}".format(tcp_ip, tcp_port))
			s.bind((tcp_ip, tcp_port))
			s.listen(5)  # Blocking
			#global conn
			conn, addr = s.accept()
			print ("Connected client address: {}".format(addr))
		else:
			# Client
			if is_tcp:
				print("Start TCP connect IP: {}:{}".format(tcp_ip, tcp_port))
				s.connect((tcp_ip, tcp_port)) # Blocking
			print("Successful connect")

		# Start threads, because ~connect is successful
		connectOk = True
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
	except KeyboardInterrupt:
		print("KeyboardInterrupt: perhaps want to exit")
		raise
	except Exception as excpt:
		print("Error in Main" + str(excpt))
		connectOk = False
		

	connectOk = False
		
	if is_tcp:
		s.close()
