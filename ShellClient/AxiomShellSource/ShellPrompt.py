#!/usr/bin/python3

import shlex

from prompt_toolkit.contrib.regular_languages.compiler import compile
from prompt_toolkit.styles import Style
from prompt_toolkit import PromptSession
from prompt_toolkit.lexers import SimpleLexer
from prompt_toolkit.history import FileHistory
from prompt_toolkit.completion import WordCompleter, PathCompleter
from prompt_toolkit.contrib.regular_languages.lexer import GrammarLexer
from prompt_toolkit.contrib.regular_languages.completion import GrammarCompleter

class ShellPrompt():
    def SetPrompt(self, prompt):
        self.prompt = prompt

    def GetPrompt(self):
        return (self.prompt)

    def __init__(self):
        self.commands = [
            "help",
            "whoami",
            "cd",
            "cat",
            "rm",
            "rmdir",
            "mkdir",
            "ls",
            "upload",
            "download",
        ]
        self.style = Style.from_dict(
            {
                "command": "#19CB00 bold",
                "quoted_string": "#CECB00",
                "string": "#0DCDCD",
                "number": "#0DCDCD",
                "local_file": "#0DCDCD underline",
                "option": "#EE5563",
                "trailing-input": "bg:#662222 #ffffff"
            }
        )
        self.g = self.CreateGrammar()
        self.lexer = GrammarLexer(
            self.g,
            lexers={
                "command": SimpleLexer("class:command"),
                "quoted_string": SimpleLexer("class:quoted_string"),
                "string": SimpleLexer("class:string"),
                "number": SimpleLexer("class:number"),
                "local_file": SimpleLexer("class:local_file"),
                "option": SimpleLexer("class:option")
            },
        )
        self.completer = GrammarCompleter(
            self.g,
            {
                "command": WordCompleter(self.commands),
                "local_file": PathCompleter(min_input_len=3)
            },
        )
        self.history = FileHistory('.axiom-shell-history')
        self.session = PromptSession(history=self.history)
        self.prompt = None

    def CreateGrammar(self):
        return compile(
            r"""
                (\s* (?P<command>(help))) |
                (\s* (?P<command>(whoami)) (\s+(?P<option>(/priv|/groups|/all)))+ \s*) |
                (\s* (?P<command>(cd)) \s+ ((?P<string>[a-zA-Z0-9:\\-_. ]+)|(?P<quoted_string>['"][a-zA-Z0-9:\\-_. ]+['"])) \s*) |
                (\s* (?P<command>(cat)) \s+ ((?P<string>[a-zA-Z0-9:\\-_. ]+)|(?P<quoted_string>['"][a-zA-Z0-9:\\-_. ]+['"])) \s*) |
                (\s* (?P<command>(rm|rmdir|mkdir)) \s+ ((?P<string>[a-zA-Z0-9:\\-_. ]+)|(?P<quoted_string>['"][a-zA-Z0-9:\\-_. ]+['"])) \s*) |
                (\s* (?P<command>(ls)) (\s+(?P<option>(-[lad]+|--)))* (\s+(?P<string>[a-zA-Z0-9:\\-_.]+|)|(?P<quoted_string>['"][a-zA-Z0-9:\\-_.$ ]+['"]))+   \s*) |
                (\s* (?P<command>(upload)) \s+ (?P<local_file>[a-zA-Z0-9-_./]+) \s+ ((?P<string>[a-zA-Z0-9:\\-_. ]+)|(?P<quoted_string>['"][a-zA-Z0-9:\\-_. ]+['"])) \s*) |
                (\s* (?P<command>(download)) (\s+(?P<string>[a-zA-Z0-9:/\\-_.]+|)|(?P<quoted_string>"[a-zA-Z0-9:/\\-_. ]+")) (\s+(?P<string>[a-zA-Z0-9:/\\-_.]+|)|(?P<quoted_string>"[a-zA-Z0-9:/\\-_. ]+")) \s*) |
                (\s* (.*)?)
            """
        )

    def Prompt(self, prompt):
        line = self.session.prompt(
            prompt,
            lexer=self.lexer,
            completer=self.completer,
            style=self.style
        )
        return (shlex.split(line))

