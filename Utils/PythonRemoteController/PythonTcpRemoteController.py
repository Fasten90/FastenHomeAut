#!/usr/bin/env python

import socket
import io
import threading
import time


# Connection configs
TCP_IP_DEFAULT = '192.168.0.199'
#TCP_IP_DEFAULT = '192.168.4.2'
#TCP_IP_DEFAULT = '192.168.1.62'
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
	
	speed_max = 50
	speed_min = -50
	turn_max = 30
	turn_min = -30
	turn = 0
	speed = 0
	send_msg = ""
	
	while connectOk:
		# TODO: How to get cursor inputs?
		#type = input() # Do nog give parameter to input, because it will re-printed on console
		print("Wait char")
		import msvcrt
		type = msvcrt.getch()
		key = ""
		if type == "x":
			print("Exit")
			connectOk = False
			needRun = False
		elif type == b'K': 
			key = "left"
		elif type == b'H': 
			key = "up"
		elif type == b'M': 
			key = "right"
		elif type == b'P': 
			key = "down"
		elif type == b'\xe0':
			continue # ~Escape
		elif type == 255:
			# No key pressed
			continue 
		else:
			print("Pressed key: {}".format(type))

		# Motor control
		if key == "left":
			if turn > turn_min:
				turn -= 5
		if key == "right":
			if turn < turn_max:
				turn += 5
		if key == "up":
			if speed < speed_max:
				speed += 5
		if key == "down":
			if speed > speed_min:
				speed -= 5

		# Create actual send message
		# Message like: "motor 30 20" --> "motor <speed> <turn>"
		send_msg = "motor {} {}".format(speed, turn)
		print(send_msg)
		send_msg = bytes(send_msg.encode("ASCII"))
		
		try:
			if is_tcp:
				if is_server:
					conn.send(send_msg)
				else:
					s.send(send_msg)
			else:
				s.sendto(msg, (tcp_ip, tcp_port))
		except Exception as excpt:
			print("Error in sending thread: " + str(excpt))
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
			global conn
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
	except Exception as excpt:
		print("Error in Main" + str(excpt))
		connectOk = False	

	connectOk = False
		
	if is_tcp:
		s.close()
