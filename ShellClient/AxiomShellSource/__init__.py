#!/usr/bin/python3

import sys
import socket
import threading

from .ShellPrompt import ShellPrompt
from .Sockets import SocketAction

HOST = '0.0.0.0'
PORT = 9001
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sprompt = ShellPrompt()

banner = """
 █████╗ ██╗  ██╗██╗ ██████╗ ███╗   ███╗███████╗██╗  ██╗███████╗██╗     ██╗
██╔══██╗╚██╗██╔╝██║██╔═══██╗████╗ ████║██╔════╝██║  ██║██╔════╝██║     ██║
███████║ ╚███╔╝ ██║██║   ██║██╔████╔██║███████╗███████║█████╗  ██║     ██║
██╔══██║ ██╔██╗ ██║██║   ██║██║╚██╔╝██║╚════██║██╔══██║██╔══╝  ██║     ██║
██║  ██║██╔╝ ██╗██║╚██████╔╝██║ ╚═╝ ██║███████║██║  ██║███████╗███████╗███████╗
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝

An OK-tier shell re-implementation


    """

def main():
    print(banner)
    if len(sys.argv) != 2:
        print(f"[!] Usage: MidShell-Client <LISTENING_PORT>")
        exit(1)

    print("[*] AxiomShell Starting up...")

    PORT = int(sys.argv[1])
    sock.bind((HOST, PORT))
    sock.listen()
    print(f"[*] Listening on {HOST}:{PORT}...")

    conn, addr = sock.accept()
    print(f"[*] Received connection from: {addr} on port {PORT}")

    tr = threading.Thread(target=SocketAction.Recv, args=(conn, sprompt,))
    ts = threading.Thread(target=SocketAction.Send, args=(sock, sprompt, conn,))
    tr.daemon = True
    ts.daemon = False
    tr.start()
    ts.start()

if __name__ == "__main__":
    main();

