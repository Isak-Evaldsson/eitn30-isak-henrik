import socket
import time
import threading

host = '192.168.0.1'
send_port = 12345 # The same port as used by the server
rec_port = 54321

nbrReplies = 0
avgLatency = 0

def timeMillis():
    val = int(time.time() * 1000)
    print(val)
    return val

def reader():
    global nbrReplies
    global avgLatency

    while True:
        data = s2.recv(1024)  
        replyTime = int.from_bytes(data, 'big')
        t = timeMillis()
        latency = t - replyTime
        nbrReplies += 1
        print(t)
        print(replyTime)
        print(latency)
        avgLatency = (avgLatency + latency) / nbrReplies
        print('Received packet with latency:', latency, 'ms, avg: ', avgLatency)


def writer():
    while True:
        s1.sendall(timeMillis().to_bytes(6, 'big'))
        time.sleep(1)
        
print("Connecting to server")
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s1.connect((host, send_port))
s2.connect((host, rec_port))

print("Starting threads")
t1 = threading.Thread(target=reader)
t2 = threading.Thread(target=writer)

t1.start()
t2.start()
t1.join()
t2.join()