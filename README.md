Naia (Dofus 1.29.1 C++ Emulator)
================================

This was made in 2010-2011 in highschool, back when I had a lot of time and my nickname was still "Nekkro". While this repository is mainly here to have a laugh making fun of my old newbie self, it is also a kind reminder to not underestimate passion and grit in beginners.

Despite the horrendous code-base, this is one of the most full-featured emulators I have ever produced.

**Note**: Back in the days, I used Code::Blocks and worked on Windows. This project has never been tested on Unix, ever. That's why the tool to install the SQL databases is in Batch. Yeah, I know. Sorry, not sorry.

## How to build

You need to install and set up Conan 1.53+, CMake 3.16+, then:

```shell
conan install -if build/Release .
conan build -if build/Release .
```

## Set up the databases

You need a MySQL server installed and running on localhost and default port (as well as in your PATH). Now run:

```shell
.\Tools\database_installer.bat
```

## Before connecting to the servers

You need:
1. to edit the Dofus client's `config.xml` file to add a `connserver` entry to localhost,
2. to insert some credentials into the LoginServer's database (look at the `accounts` table), and
3. to register the game server into the LoginServer's database so it appears during server selection.

## Starting the servers

Review and modify the configuration files under `LoginServer/config` and `GameServer/config`. Then:

```shell
START "" /D LoginServer .\build\LoginServer\Release\LoginServer.exe
START "" /D GameServer .\build\GameServer\Release\GameServer.exe
```
