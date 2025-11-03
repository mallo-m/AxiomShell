#!/usr/bin/python3

import os
import struct
import base64

class FileHandlers():
    @staticmethod
    def Upload(ddata, conn):
        print("[+] Target is ready for upload, calculating length...")
        local_filepath:str = base64.b64decode(ddata['local_filepath']).decode()

        if (local_filepath.startswith(":ASSET:")):
            local_filepath = os.path.realpath(os.path.dirname(__file__) + "/../" + local_filepath.split(':')[-1])
        else:
            pass

        print(f"[+] Reading {local_filepath}")
        f = open(local_filepath, "rb")
        raw_content = f.read()
        f.close()

        print(f"[+] File has {len(raw_content)} bytes")
        conn.send(struct.pack('<Q', len(raw_content)))
        conn.send(raw_content)
        try:
            return
        except PermissionError:
            print("[!] Failed to open file: Permission Denied")
        except FileNotFoundError:
            print("[!] Failed to open file: No such file or directory")
        except:
            print("[!] Failed to open file: Unknown error")

        raw_content = b''
        conn.send(str(len(raw_content)).encode())
        ack = conn.recv(4)

    @staticmethod
    def Download(ddata, conn):
        print("[+] Target is ready to serve file")
        local_filepath = base64.b64decode(ddata['local_filepath'])
        filelen = ddata['filelen']

        print(f"[+] Saving to {local_filepath}")
        print(f"[+] File will be {filelen} bytes long")

        bcount = 0
        f = open(local_filepath, "wb")
        while (bcount < filelen):
            tmp = conn.recv(min(4096, filelen - bcount))
            f.write(tmp)
            bcount += len(tmp)
        f.close()

        print("[+] File written !")

