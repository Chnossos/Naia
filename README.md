Naia (Dofus 1.29.1 C++ Emulator)
================================

This was made in 2010-2011 in highschool, back when I had a lot of time and my nickname was still "Nekkro". While this repository is mainly here to have a laugh making fun of my old newbie self, it is also a kind reminder to not underestimate passion and grit in beginners.

Despite the horrendous code-base, this is one of the most full-featured emulators I have ever produced while being a huge beginner that had to spend more than 2 weeks just to link with Boost.Asio.

**Note**: Back in the days, I used Code::Blocks and worked on Windows. This project has never been tested on Unix, ever. That's why the tool to install the SQL databases is in Batch. Yeah, I know. Sorry, not sorry.

## Features

* Builds with MSVC 16 (which corresponds to Visual Studio 2019)
* Only works with *Dofus 1.29.1*, **NOT** *Dofus Retro*! I'm not telling you where to find it.

## How to build

You need to install some VS Build Tools and set up Conan 1.53+, CMake 3.16+ (and Qt5 for GSRegister), then:

```shell
conan install -if build/Release .
conan build -if build/Release .
```

## Set up the databases

You need a MySQL server installed and running on localhost and default port (as well as in your PATH). Now run this and follow the instructions:

```shell
.\Tools\database_installer.bat
```

## Before connecting to the servers

1. Register the game server into the LoginServer's database so it appears during server selection:
   1. Run the GSRegister program, select the server ID you want and generate the `hexid` file. You can now insert it into the `gameservers` table:

      ```shell
      mysql.exe -h localhost -u root -p -D naia -e 'INSERT INTO gameservers VALUES(2, "<hexid_content>");' 
      ```

   2. Put the `hexid(X).txt` file into the `config` folder of game server, and rename it `hexid.txt`.

2. Edit the Dofus client's `config.xml` file to add a `connserver` entry to localhost under the `conf` XML node:

   ```xml
   <connserver name="Local" ip="localhost" port="8443"/>
   ```

## Starting the servers

Review and modify the configuration files under `LoginServer/config` and `GameServer/config`. Then:

```shell
START "" /D LoginServer ..\build\LoginServer\Release\LoginServer.exe
START "" /D GameServer ..\build\GameServer\Release\GameServer.exe
```
