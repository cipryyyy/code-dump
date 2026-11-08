import socket
import time
from tqdm import trange
import threading

HOST = "192.168.1.200"
PORT = 6789
PKG_SIZE = 76
ITERATIONS = 5000

def tester():
    t = []

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

        print(f"Package size = {PKG_SIZE}")
        print(f"Iterations = {ITERATIONS}\n")

        pbar = trange(ITERATIONS, unit="pkg")

        for it in pbar:
            pbar.set_description_str(f"pkg #{it + 1}")
            s.send(b't')

            vo = 2
            v = vo.to_bytes(PKG_SIZE, "little")

            s.send(v)
            s.send(b'r')

            start = time.time()
            data = int.from_bytes(s.recv(PKG_SIZE), 'little')
            t.append(time.time() - start)

        avg = sum(t) / len(t)
        print(f"{round(avg, 5)}s/pkg [{round(1/avg, 3)}Hz]")


if __name__ == "__main__":
    threads = []
    for _ in range(10):
        th = threading.Thread(target=tester)
        threads.append(th)

    for th in threads:
        th.start()

    for th in threads:
        th.join()
