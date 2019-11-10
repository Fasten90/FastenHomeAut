#!/usr/bin/env python

import socket
import io
import threading
import time

import keyboard


# Connection configs
TCP_IP_DEFAULT = '192.168.0.241'
#TCP_IP_DEFAULT = '192.168.0.199'
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

login_received = False

# Motor control    
speed_max = 50
speed_min = -50
speed_step = 10

turn_max = 30
turn_min = -30
turn_step = 10

send_msg = ""



speed = 0
turn = 0

prev_speed = 0
prev_turn = 0

key_check_sec = 0.05  # 50ms
idle_count_limit = 5  # ide limit --> 5* 0.05sec = 250 ms
idle_count_dc = 0
idle_count_turn = 0

force_send_msg = False


def speed_handle(speed_dir):
    global speed
    if speed_dir > 0:
        speed += speed_step
    elif speed_dir < 0:
        speed -= speed_step
    else:
        print("Error: speed_dir")
    if speed > speed_max:
        speed = speed_max
    elif speed < speed_min:
        speed = speed_min

def turn_handle(turn_dir):
    global turn
    if turn_dir > 0:
        turn += turn_step
    elif turn_dir < 0:
        turn -= turn_step
    else:
        print("Error: turn_dir")
    if turn > turn_max:
        turn = turn_max
    elif turn < turn_min:
        turn = turn_min

def keyboard_handle_thread():

    global connectOk
    global needRun
    
    global send_msg
    global login_received

    key_left_is_pressed = False
    key_up_is_pressed = False
    key_right_is_pressed = False
    key_down_is_pressed = False
    key_left_is_pressed_prev = False
    key_up_is_pressed_prev = False
    key_right_is_pressed_prev = False
    key_down_is_pressed_prev = False
    
    key_speed_is_pressed = False
    key_turn_is_pressed = False

    key_changed = False
    idle_status = False

    started = False

    while True:  # making a loop

        try:  # used try so that if user pressed other than the given key error will not be shown
            #print('You Pressed a Key!')
            time.sleep(key_check_sec)
            key_pressed = False
            key_speed_is_pressed = False
            key_turn_is_pressed = False

            if keyboard.is_pressed('left arrow') or keyboard.is_pressed('a'):
                print ('Left')
                turn_handle(1)
                key_pressed = True
                key_left_is_pressed = True
                key_turn_is_pressed = True
            else:
                key_left_is_pressed = False
            if keyboard.is_pressed('up arrow') or keyboard.is_pressed('w'):
                print ('Up')
                speed_handle(1)
                key_pressed = True
                key_up_is_pressed = True
                key_speed_is_pressed = True
            else:
                key_up_is_pressed = False
            if keyboard.is_pressed('right arrow') or keyboard.is_pressed('d'):
                print ('Right')
                turn_handle(-1)
                key_pressed = True
                key_right_is_pressed = True
                key_turn_is_pressed = True
            else:
                key_right_is_pressed = False
            if keyboard.is_pressed('down arrow') or keyboard.is_pressed('s'):
                print ('Down')
                speed_handle(-1)
                key_pressed = True
                key_down_is_pressed = True
                key_speed_is_pressed = True
            else:
                key_down_is_pressed = False

            # Check changes
            if key_left_is_pressed != key_left_is_pressed_prev:
                key_changed = True
            elif key_up_is_pressed != key_up_is_pressed_prev:
                key_changed = True
            elif key_right_is_pressed != key_right_is_pressed_prev:
                key_changed = True
            elif key_down_is_pressed != key_down_is_pressed_prev:
                key_changed = True
            else:
                key_changed = False
                #print("Key change")

            # Save new statuses
            key_left_is_pressed_prev = key_left_is_pressed
            key_up_is_pressed_prev = key_up_is_pressed
            key_right_is_pressed_prev = key_right_is_pressed
            key_down_is_pressed_prev = key_down_is_pressed

            if keyboard.is_pressed('left ctrl') or keyboard.is_pressed('x'):  # if key 'q' is pressed 
                print('Exit from key handler because Ctrl or x')
                connectOk = False
                needRun = False
                break  # finishing the loop


            #if not key_pressed:
            if not key_speed_is_pressed or not key_turn_is_pressed:
                # one direction is unpressed
                global idle_count_dc
                global idle_count_turn
                global speed
                global turn

                if not key_speed_is_pressed:
                    idle_count_dc += 1
                if not key_turn_is_pressed:
                    idle_count_turn += 1

                idle_speed = False
                idle_turn = False

                if idle_count_dc >= idle_count_limit:
                    # Idle event
                    speed = 0
                    idle_speed = True
                
                if idle_count_turn >= idle_count_limit:
                    # Idle event
                    turn = 0
                    idle_turn = True
                
                if idle_speed and idle_turn:
                    # Full idle
                    if not idle_status:
                        # Force send message
                        force_send_msg = True
                        print("Idle event")
                        idle_status = True
                else:
                    # not pressed, send not message
                    pass

            if key_speed_is_pressed or key_turn_is_pressed:
                # One key pressingPressing
                idle_status = False
                if key_speed_is_pressed:
                    idle_count_dc = 0
                if key_turn_is_pressed:
                    idle_count_turn = 0
            
            if key_changed:
                print("Key changed")
                # Force send message
                force_send_msg = True
                key_changed = False
                    
        except Exception as ex:
            print("Exit from keyhandler")
            print(str(ex))
            connectOk = False
            needRun = False
            break  # if user pressed a key other than the given key the loop will break

        if not started and connectOk:
            started = True
            print("Started run")
        if started and not needRun:
            print("Exit, requested by another thread")
            break


def tcp_send_thread():
    global connectOk
    global needRun
    global s
    global is_tcp
    global login_received
    global force_send_msg

    # Wait
    time.sleep(1)
    timeout_calc = 0

    while connectOk:
        # Create actual send message
        # Message like: "motor 30 20" --> "motor <speed> <turn>"

        if timeout_calc > 4 or force_send_msg:
            
            timeout_calc = 0
            if force_send_msg:
                force_send_msg = False

            if login_received:
                send_msg = "motor {} {}\r\n".format(speed, turn)
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
            else:
                print("Message wasn't sent (not received login msg)")
            
        # Delay
        time.sleep(0.05)
        timeout_calc += 1
        
        if not needRun:
            print("Exit requested by another thread")

    print("Exit Send thread")
    

def tcp_receive_thread():
    global connectOk
    global needRun
    global s
    global conn
    global is_tcp
    global login_received

    while connectOk:
        try:
            if is_server:
                data = conn.recv(BUFFER_SIZE)
            else:
                data = s.recv(BUFFER_SIZE)
            if not data:
                print("Received closing byte, pls reconnect!")
                break
            elif data == b'login':
                login_received = True
                print("Login message received")
            print("Received data: {}".format(str(data)))
        except Exception as excpt:
            # TODO: Handle WinError
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
    login_received = False
    
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
        threadKeyHandler = threading.Thread(name="Send thread", target=keyboard_handle_thread)
        threadKeyHandler.start()

        while connectOk:
            # If everything is ok
            try:
                time.sleep(1)
                if s._closed:
                    connectOk = False
                    break
            except KeyboardInterrupt:
                print("Keyboard interrupt, exit")
                connectOk = False
                needRun = False
            except Exception as ex:
                print(str(ex))
                raise ex
    except Exception as excpt:
        print("Error in Main" + str(excpt))
        connectOk = False

    connectOk = False
        
    if is_tcp:
        s.close()
