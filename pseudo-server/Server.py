import socket
import threading

def handle_client(c, a):
    c.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1) #Disable Nagle alg
    global value

    try:
        while True:
            cmd = c.recv(1)         #t or r cmd
            if not cmd:
                break
            
            if cmd == b't':
                data = c.recv(4)    # receive new data
                value = int.from_bytes(data, "little")
            elif cmd == b'r':       # transmit stored data
                c.sendall(value.to_bytes(4, 'little'))
            else:
                break
    except Exception:
        pass
    finally:
        print(f"{a} disconnected")
        c.close()

def main():
    HOST = "192.168.1.200"
    PORT = 6789

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5)
        print(f"Server running on {HOST}:{PORT}")

        try:
            while True:
                conn, addr = s.accept()
                conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                print(f"Connection detected: ({addr[0]}:{addr[1]})")
                
                t = threading.Thread(target=handle_client, args=(conn, addr))
                t.daemon = True
                t.start()

        except KeyboardInterrupt:
            print("\nServer stopped.")

if __name__ == "__main__":
    main()