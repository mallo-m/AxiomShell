#!/usr/bin/python3

class Utils:
    HELP_USAGE = """
    Main usage
    ----------
    <binary_name> [{arg1}, {arg2}, ...]

    Examples
    --------
    whoami /all
    ping.exe -n 1 192.168.57.1
    tasklist

    Shell builtins
    --------------
    help                                                Displays this message
    whoami {/all} {/priv} {/groups}                     Builtin re-implementation of whoami.exe. Also reflects impersonation status
    cd <PATH>                                           Change current working directory to the specified path. Accounts for impersonation
    cat <PATH>                                          Printfs a file's content to the terminal. Prefer the download builtin for heavy or binary files.
    rm [{arg1}, {arg2}, ...]                            Deletes a file from disk, does not work with directories (sorry). Accounts for impersonation
    rmdir [{arg1}, {arg2}, ...]                         Deletes a directory from disk. Directory must be empty.
    mkdir [{arg1}, {arg2}, ...]                         Creates a directory.
    ls [<PATH>]                                         List files in the current directory or the specified directory. Doesn't work when passing a file as arg. Accounts for impersonation
    upload <LOCAL_PATH> <REMOTE_PATH>                   Uploads a local file to the specified location on the target
    download <REMOTE_PATH>                              Downloads a remote file to local storage

    Shell builtins examples
    -----------------------
    cd C:\\\\Temp
    cd 'C:\\Temp'

    rm C:/Programdata/totslegit.exe

    ls
    ls ../
    ls 'C:\\Users'

    Upload malicious.sys 'C:\\Temp\\malicious.sys'
    Download C:\\\\Temp\\\\SAM.save
        """

