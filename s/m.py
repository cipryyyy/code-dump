import socket
import time

"""
129.195Hz in recv
~
"""

HOST = "192.168.1.200"
PORT = 6789

t = []

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

    for _ in range(1000):
        s.send(b't')

        vo = 2
        v = vo.to_bytes(4, "little")

        s.send(v)
        s.send(b'r')

        start = time.time()
        data = int.from_bytes(s.recv(4), 'little')
        t.append(time.time() - start)

    avg = sum(t)/len(t)
    print(f"{round(avg, 5)}s [{round(1/avg, 3)}Hz]")
