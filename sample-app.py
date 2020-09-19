#!/usr/bin/env python

import sys
import socket
import threading
import string
import struct

class Sample:

    def __init__(self):
        self.running = True

    def send_all_bytes(self, sock, bytes):
        bytes_left = len(bytes)
        while bytes_left > 0:
            bytes_sent = sock.send(bytes) # sock.send returns the actual bytes sent
            bytes_left = bytes_left - bytes_sent
            bytes = bytes[bytes_sent:]

    def recv_message(self, sock):
        size = sock.recv(4)
        size = socket.ntohl(struct.unpack('i', size)[0])
        response = sock.recv(size).decode()
        return response

    def send_message(self, sock, message):
        packet = message.encode()
        packet = struct.pack('i', socket.htonl(len(packet))) + packet
        self.send_all_bytes(sock, packet)
        return True

    def handle_client(self, sock, addr):
        try:
            while True:
                message  = self.recv_message(sock)
                print(addr, "Got message", message)
                if message == "close":
                    response = "Understood, closing socket"
                else:
                    response = "Message received: '" + message + "', length: " + str(len(message))
                self.send_message(sock, response)

                if message == "close":
                    break

            print(addr, "Done with this client, closing socket")
        except:
            e = sys.exc_info()
            print(addr, "Socket error", str(e), ", aborting")

        sock.close()

    def start_server(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.bind(("0.0.0.0", 9001))
        sock.listen()
        print("Server started")

        try:
            while self.running:
                client, addr = sock.accept()
                print("New connection from", str(addr))
                t = threading.Thread(target = self.handle_client, args = (client, addr))
                t.start()
        except:
            e = sys.exc_info()
            print("Socket error", str(e), ", aborting")

        sock.close()
        return 0

    def stop_server(self):
        self.running = False


    def start_client(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect(("127.0.0.1", 9001))

        try:
            while True:
                line = input("Write a text to send to the server: ")
                self.send_message(sock, line)
                response = self.recv_message(sock)
                print("Server reponse:", response)

                if line == "close":
                    break

            print("Done here..")
        except:
            e = sys.exc_info()
            print("Socket error", str(e), ", aborting")

        sock.close()
        return 0

def usage():
    print("usage: " + sys.argv[0] + " client|server")
    exit(1)

if __name__ == "__main__":
    if len(sys.argv) == 2:
        if sys.argv[1] == "server":
            s = Sample()
            s.start_server()
        elif sys.argv[1] == "client":
            print("will run as client")
            c = Sample()
            c.start_client()
        else:
            usage()
    else:
        usage()
