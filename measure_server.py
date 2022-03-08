import socket

host = ''        # Symbolic name meaning all available interfaces
rec_port = 12345     # Arbitrary non-privileged port
send_port = 54321

s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s1.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s2.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

s1.bind((host, rec_port))
s2.bind((host, send_port))

s1.listen(1)
s2.listen(1)

conn1, addr = s1.accept()
conn2, _ = s2.accept()

print('Connected by', addr)
while True:

    try:
        data = conn1.recv(1024)

        if not data: break
        
        # Echoes back the conntent of the packet to the other socket
        conn2.sendall(data)

    except socket.error:
        print("Error Occured.")
        break

conn1.close()
conn2.close()