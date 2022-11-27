@ECHO off

SET config_file=config.txt
SET cmode=n
SET install_type=f

REM ================================

:loadconfig
TITLE Naia DB Installer - Loading configuration
CLS
IF NOT EXIST %config_file% GOTO configure
REN %config_file% config.bat
CALL config.bat
REN config.bat %config_file%
IF /i "%cmode%"=="i" GOTO ls_backup
GOTO end

REM ================================

:configure
TITLE Naia DB Installer - MySQL setup
COLOR 07
CLS
IF /i "%cmode%"=="n" (
    REM LoginServer
    SET ls_host=localhost
    SET ls_name=naia
    SET ls_user=root
    SET ls_pass=
    REM GameServer
    SET gs_host=localhost
    SET gs_name=naia
    SET gs_user=root
    SET gs_pass=
    REM System
    SET cmode=i
)
REM === FILL DATA ===
ECHO New settings will be created for this tool to run in
ECHO your computer ; please answer following questions.
ECHO.
ECHO 1 - MySQL Binaries
ECHO --------------------
ECHO In order to perform the tool's tasks, the path for commands
ECHO such as 'mysql' and 'mysqldump' is needed. Both executables are
ECHO usually stored in the same place.
ECHO.
ECHO Attempting to find MySQL PATH...
ECHO.
IF NOT "%MySQL%"=="" (
    ECHO MySQL found in your PATH, this will be used as default.
    ECHO If you want to use something different, change following path for 
    ECHO something else.
    SET mysqlPath=%MySQL%
) ELSE (
    ECHO "MySQL" can't be found in your PATH environment variables.
    ECHO Go and find out where "mysql.exe" and "mysqldump.exe" are.
    ECHO.
    ECHO If you have no idea about the meaning of words such as MYSQL
    ECHO or PATH, you'd better close this window, and consider googling
    ECHO and reading about it. Setup and host a Naia server requires a
    ECHO minimum of technical skills.
)
:ask_for_mysql_bin_path
ECHO.
ECHO Please enter the path where these binaries are located (no trailing slash needed).
SET /p mysqlPath="(default is '%mysqlPath%'): "
SET mysqlBinPath=%mysqlPath%\mysql.exe
IF NOT EXIST "%mysqlBinPath%" (
    ECHO Binaries NOT found!
    GOTO ask_for_mysql_bin_path
)
CLS
ECHO 2 - Login Server settings
ECHO --------------------
ECHO A connection to the MySQL server you specify will be etablished,
ECHO and a Login Server database will be setup there.
ECHO Press 'enter' whitout typing value in order to use default values.
ECHO.
SET /P ls_host="Host     (default is '%ls_host%'): "
SET /P ls_name="Database (default is '%ls_name%'): "
SET /P ls_user="Username (default is '%ls_user%'): "
SET /P ls_pass="Password (will be shown as you type, default '%ls_pass%'): "
IF NOT "%ls_user%"=="%gs_user%" SET gs_user=%ls_user%
IF NOT "%ls_pass%"=="%gs_pass%" SET gs_pass=%ls_pass%
IF NOT "%ls_name%"=="%gs_name%" SET gs_name=%ls_name%
IF NOT "%ls_host%"=="%gs_host%" SET gs_host=%ls_host%
ECHO.
ECHO 3 - Game Server settings
ECHO --------------------
ECHO Use default values to combine Login and Game servers database (press 'enter').
ECHO.
SET /P gs_host="Host     (default is '%gs_host%'): "
SET /P gs_name="Database (default is '%gs_name%'): "
SET /P gs_user="User     (default is '%gs_user%'): "
SET /P gs_pass="Pass     (default is '%gs_pass%'): "

REM ================================

:create_config
SET mysqlDumpPath=%mysqlPath%\mysqldump.exe
SET backup=backup

ECHO SET cmode=%cmode%>>%config_file%
ECHO SET mysqlPath=%mysqlPath%>>%config_file%
ECHO SET mysqlBinPath=%mysqlBinPath%>>%config_file%
ECHO SET mysqlDumpPath=%mysqlDumpPath%>>%config_file%
ECHO SET gs_host=%gs_host%>>%config_file%
ECHO SET gs_name=%gs_name%>>%config_file%
ECHO SET gs_user=%gs_user%>>%config_file%
ECHO SET gs_pass=%gs_pass%>>%config_file%
ECHO SET ls_host=%ls_host%>>%config_file%
ECHO SET ls_name=%ls_name%>>%config_file%
ECHO SET ls_user=%ls_user%>>%config_file%
ECHO SET ls_pass=%ls_pass%>>%config_file%
ECHO SET backup=%backup%>>%config_file%
ECHO.
ECHO Script setup complete, your settings were saved in the
ECHO '%config_file%' file. Remember that your passwords are stored as clear text.
ECHO.
ECHO If you want to modify some values, do it before continue.
ECHO.
PAUSE
GOTO loadconfig

REM ================================

:ls_backup
TITLE Naia DB Installer - Login Server database setup
COLOR 07
CLS
ECHO Trying to make a backup of your loginserver database.
SET MYSQL_PWD=%ls_pass%
IF NOT EXIST "%backup%" MKDIR %backup%
"%mysqlDumpPath%" --add-drop-table -h %ls_host% -u %ls_user% %ls_name% > "%backup%\loginserver_backup.sql" 2> NUL
IF %ERRORLEVEL%==0 (
    ECHO Backup finished.
    ECHO.
    GOTO ls_db_ok
)
:ls_error1
TITLE Naia DB Installer - Login Server database setup - ERROR!!!
COLOR 47
ECHO.
ECHO Backup attempt failed! A possible reason for this to happen,
ECHO is that your DB doesn't exist yet. We could try to create '%ls_name%' for you.
ECHO.
:ls_ask1
SET lsdbprompt=y
ECHO ATTEMPT TO CREATE LOGINSERVER DATABASE:
ECHO.
ECHO (y) Yes
ECHO.
ECHO (n) No
ECHO.
ECHO (r) Reconfigure
ECHO.
ECHO (q) Quit
ECHO.
SET /p lsdbprompt="Choose (default Yes): "
IF /i %lsdbprompt%==y GOTO ls_db_create
IF /i %lsdbprompt%==n GOTO gs_backup
IF /i %lsdbprompt%==r (SET cmode=n & GOTO configure)
IF /i %lsdbprompt%==q GOTO end
GOTO ls_ask1

REM ================================

:ls_db_create
TITLE Naia DB Installer - Login Server database setup - DB Creation
COLOR 07
ECHO.
ECHO Trying to create a Login Server database...
SET MYSQL_PWD=%ls_pass%
"%mysqlBinPath%" -h %ls_host% -u %ls_user% -e "CREATE DATABASE `%ls_name%` DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_unicode_ci"
IF %ERRORLEVEL%==0 (
    ECHO Database created successfully!
    PAUSE
    SET install_type=f
    GOTO gs_backup
)
:gs_err2
TITLE Naia DB installer - Login Server database setup - DB Creation failed!
COLOR 47
CLS
ECHO An error occured while trying to create a database for 
ECHO your login server.
ECHO.
ECHO Possible reasons:
ECHO 1-You provided innacurate info, check username, pass, etc.
ECHO 2-User '%ls_user%' don't have enough privileges for database creation.
ECHO 3-Database exists already...?
ECHO.
ECHO I'd suggest you to look for correct values and try this
ECHO script again later. But you can try to reconfigure it now.
ECHO.
:ask_gs_db_create
SET login_errprompt=q
ECHO (r) Restart script with fresh configuration values
ECHO.
ECHO (q) Quit now
ECHO.
SET /p login_errprompt="Choose (default quit): "
IF /i %login_errprompt%==r (SET cmode=n & GOTO configure)
IF /i %login_errprompt%==q GOTO error_end
CLS
GOTO ask_gs_db_create

REM ================================

:ls_install
TITLE L2JDP installer - Login Server database setup - Full install
COLOR 07
ECHO.
ECHO Deleting loginserver tables for new content.
SET MYSQL_PWD=%ls_pass%
"%mysqlBinPath%" -h %ls_host% -u %ls_user% -D %ls_name% < %~dp0\login_install.sql 2> NUL
IF %ERRORLEVEL%==0 (
    ECHO Loginserver tables were deleted!
    ECHO.
    PAUSE
    GOTO gs_backup
)
ECHO ERROR!
GOTO end

REM ================================

:ls_db_ok
TITLE Naia DB installer - Login Server database setup - WARNING!!!
COLOR 17
:asklogin
SET loginprompt=s
ECHO LOGINSERVER DATABASE install type:
ECHO.
ECHO (f) Full: Will destroy data in your `accounts` and
ECHO     and `gameservers` tables.
ECHO.
ECHO (s) Skip: Will conserve all your data and take you to
ECHO     the Game Server database installation and upgrade options.
ECHO.
ECHO (r) Reconfigure: You'll be able to redefine MySQL path,
ECHO     user and database informations and start over with
ECHO     those fresh values.
ECHO.
ECHO (q) Quit
ECHO.
SET /p loginprompt="Choose (default '%loginprompt%'): "
IF /i "%loginprompt%"=="f" (
    SET install_type=f
    GOTO ls_install
)
IF /i "%loginprompt%"=="s" GOTO gs_backup
IF /i "%loginprompt%"=="r" GOTO configure
IF /i "%loginprompt%"=="q" GOTO end
CLS
GOTO asklogin

REM ================================

:gs_backup
TITLE Naia DB Installer - Game Server database setup
COLOR 07
CLS
IF /i "%ls_name%"=="%gs_name%" GOTO gs_db_ok
SET MYSQL_PWD=%gs_pass%
"%mysqlBinPath%" -h %gs_host% -u %gs_user% -e "ALTER DATABASE `%gs_name%` DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_unicode_ci" 2> NUL
ECHO Trying to make a backup of your gameserver database.
IF NOT EXIST "%backup%" MKDIR %backup%
"%mysqlDumpPath%" --add-drop-table -h %gs_host% -u %gs_user% %gs_name% > "%backup%\gameserver_backup.sql" 2> NUL
IF %ERRORLEVEL%==0 (
    ECHO Backup finished.
    ECHO.
    GOTO gs_db_ok
)
:gs_error1
TITLE Naia DB Installer - Game Server database setup - ERROR!!!
COLOR 47
ECHO.
ECHO Backup attempt failed! A possible reason for this to happen,
ECHO is that your DB doesn't exist yet. We could try to create '%gs_name%' for you.
ECHO.
:gs_ask1
SET lsdbprompt=y
ECHO ATTEMPT TO CREATE GAMESERVER DATABASE:
ECHO.
ECHO (y) Yes
ECHO.
ECHO (r) Reconfigure
ECHO.
ECHO (q) Quit
ECHO.
SET /p lsdbprompt="Choose (default yes): "
IF /i "%lsdbprompt%"=="y" GOTO gs_db_create
IF /i "%lsdbprompt%"=="r" (SET cmode=n & GOTO configure)
IF /i "%lsdbprompt%"=="q" GOTO end
CLS
GOTO gs_ask1

REM ================================

:gs_db_create
TITLE Naia DB Installer - Game Server database setup - DB Creation
COLOR 07
ECHO.
ECHO Trying to create a Game Server database...
SET MYSQL_PWD=%gs_pass%
"%mysqlBinPath%" -h %gs_host% -u %gs_user% -e "CREATE DATABASE `%gs_name%` DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_unicode_ci"
IF %ERRORLEVEL%==0 (
    ECHO Database created successfully!
    ECHO.
    PAUSE
    GOTO upgrade_install
)
:gs_err2
TITLE Naia DB installer - Game Server database setup - DB Creation failed!
COLOR 47
CLS
ECHO An error occured while trying to create a database for 
ECHO your game server.
ECHO.
ECHO Possible reasons:
ECHO 1-You provided innacurate info, check username, pass, etc.
ECHO 2-User '%gs_user%' don't have enough privileges for database creation.
ECHO 3-Database exists already...?
ECHO.
ECHO I'd suggest you to look for correct values and try this
ECHO script again later. But you can try to reconfigure it now.
ECHO.
:ask_gs_db_create
SET game_errprompt=q
ECHO (r) Restart script with fresh configuration values
ECHO.
ECHO (q) Quit now
ECHO.
SET /p game_errprompt="Choose (default quit): "
IF /i "%game_errprompt%"=="r" (SET cmode=n & GOTO configure)
IF /i "%game_errprompt%"=="q" GOTO error_end
CLS
GOTO ask_gs_db_create

REM ================================

:gs_install
TITLE Naia DB installer - Game Server database setup - Full install
COLOR 07
ECHO.
ECHO Deleting gameserver tables for new content.
SET MYSQL_PWD=%gs_pass%
"%mysqlBinPath%" -h %gs_host% -u %gs_user% -D %gs_name% < %~dp0\game_install.sql
IF %ERRORLEVEL%==0 (
    ECHO Gameserver tables were deleted.
    ECHO.
    PAUSE
    GOTO upgrade_install
)
ECHO ERROR!
GOTO error_end

REM ================================

:gs_db_ok
TITLE Naia DB installer - Game Server database setup - WARNING!!!
COLOR 17
:askgame
IF /i "%install_type%"=="f" (SET gameprompt=f) ELSE (SET gameprompt=u)
ECHO GAMESERVER DATABASE install type:
ECHO.
ECHO (f) Full: WARNING! Will destroyes ALL of your existing character
ECHO     data (it really means it: items, pets.. ALL)
ECHO.
ECHO (u) Upgrade: We'll do our best to preserve all of your character
ECHO     data. /!\ SERVICE UNAVAILABLE /!\
ECHO.
ECHO (q) Quit
ECHO.
SET /p gameprompt="Choose (default '%gameprompt%'): "
IF /i "%gameprompt%"=="f" GOTO gs_install
IF /i "%gameprompt%"=="u" GOTO end
REM IF /i "%gameprompt%"=="u" SET install_type=u&GOTO upgrade_install
IF /i "%gameprompt%"=="q" GOTO end
CLS
GOTO askgame

REM ================================

:upgrade_install
COLOR 07
CLS
SET MYSQL_PWD=%gs_pass%
"%mysqlBinPath%" -h %gs_host% -u %gs_user% -e "ALTER DATABASE `%gs_name%` DEFAULT CHARACTER SET utf8 DEFAULT COLLATE utf8_unicode_ci"
IF /i "%install_type%"=="f" (
    TITLE Naia DB installer - Game Server database setup - Installing...
    ECHO Installing new gameserver content...
    ECHO Please wait for another instruction before continue!
    ECHO It may takes several minutes, especially for experience and maps!
) ELSE (
    TITLE Naia DB installer - Game Server database setup - Upgrading...
    ECHO Upgrading gameserver content...
    ECHO Please wait for another instruction before continue!
    ECHO It may takes several minutes, especially for experience and maps!
)
ECHO ================================
IF "%loginprompt%"=="f" (
    SET dest=ls
    FOR %%i IN (sql\login\*.sql) DO CALL:dump %%i
)
SET dest=gs
FOR %%i IN (sql\game\*.sql) DO CALL:dump %%i

ECHO Done...
ECHO.
PAUSE
GOTO update_sql

REM ================================

:dump
IF /i "%install_type%"=="f" (SET action=Installing) ELSE (SET action=Upgrading)
ECHO %action% %~nx1
IF "%dest%"=="ls" (
    SET MYSQL_PWD=%ls_pass%
    "%mysqlBinPath%" -h %ls_host% -u %ls_user% -D %ls_name% < %1
)
IF "%dest%"=="gs" (
    SET MYSQL_PWD=%gs_pass%
    "%mysqlBinPath%" -h %gs_host% -u %gs_user% -D %gs_name% < %1
)
IF NOT %ERRORLEVEL%==0 (PAUSE & CALL:sql_error %1)
GOTO:eof

REM ================================

:sql_error
CLS
TITLE Naia DB installer - potential database issue
ECHO Something caused an error while executing instruction:
ECHO %cmdline%
ECHO.
ECHO with file %~nx1
ECHO.
ECHO What we should do now?
ECHO.
:ask_sql
SET ntpebcak=c
ECHO (c) Continue: Let's pretend that nothing happened and continue with
ECHO     the rest of the list.
ECHO.
ECHO (r) Reconfigure: Perhaps these errors were caused by a typo.
ECHO     You can restart from scratch and redefine paths, databases
ECHO     and user info again.
ECHO.
ECHO (q) Quit now
ECHO.
SET /p ntpebcak="Choose (default continue): "
IF  /i "%ntpebcak%"=="c" goto:eof
IF  /i "%ntpebcak%"=="r" call:configure
IF  /i "%ntpebcak%"=="q" call:error_end
GOTO ask_sql

REM ================================

:update_sql
TITLE Naia DB installer - Game Server database setup - update SQL files
COLOR 07
IF "%install_type%"=="f" GOTO end
CLS
ECHO In the sql/server/updates , we use to store cummulative changes
ECHO needed by the database structures.
ECHO.
ECHO Usually these SQL files are created whenever some new
ECHO feature implementation requires it.
ECHO.
ECHO If you're too lame to know what these changes are about,
ECHO we can try to apply these patches for you.
:asknb
SET nbprompt=a
ECHO.
ECHO What we do with the .sql files in your updates folder ?
ECHO.
ECHO (a) All Login/Game Updates: We'll do the automatic process
ECHO     with LS/CS/GS updates folders.
ECHO.
ECHO (l) Only Login Updates: We'll do the automatic process
ECHO     only with the sql/login/updates folder.
ECHO.
ECHO (g) Only Game Updates: We'll do the automagic process
ECHO     only with the sql/game/updates folder.
ECHO.
ECHO (s) Skip: We'll do nothing, it's up to you to find out
ECHO     which file does what, which one could be of use for
ECHO     you, etc.
ECHO.
SET /p nbprompt=" Choose (default auto-all): "
IF /i "%nbprompt%"=="a" GOTO nb_ls_install
IF /i "%nbprompt%"=="l" GOTO nb_ls_install
IF /i "%nbprompt%"=="g" GOTO nb_gs_install
IF /i "%nbprompt%"=="s" GOTO end
CLS
GOTO asknb
:nb_ls_install
FOR %%i IN (sql\login\updates\*.sql) DO "%mysqlBinPath%" -h %ls_host% -u %ls_user% -D %ls_name% < %%i
IF /i %nbprompt%==l GOTO nbfinished
:nb_gs_install
FOR %%i IN (sql\game\updates\*.sql) DO "%mysqlBinPath%" -h %gs_host% -u %gs_user% -D %gs_name% < %%i
PAUSE
:nbfinished
TITLE Naia DB installer - Game Server database setup - updates processing complete
COLOR 07
CLS
ECHO Automagic processing finished.
ECHO.
ECHO Remember that some of these files could have tried to add stuff that were 
ECHO part of your database structure already, so don't go out yelling about 
ECHO.
ECHO 'Duplicate column name'
ECHO.
ECHO messages you may find there.
ECHO.
ECHO Rather you should focus in those that say 
ECHO.
ECHO 'Table doesn't exist'
ECHO.
ECHO for example.
ECHO.
GOTO end

REM ================================

:error_end
TITLE Naia DB installer - oops...
COLOR 47
CLS
ECHO That wasn't a clear run but don't worry,
ECHO you can get help and support:
ECHO.
ECHO 1 - Read about the Naia Project
ECHO        [ https://github.com/Chnossos/Naia ]
ECHO.
COLOR 07
EXIT

REM ================================

:end
TITLE Naia DB Installer - Script execution finished
COLOR 07
CLS
ECHO Thanks for using our software.
ECHO.
ECHO Visit [ https://github.com/Chnossos/Naia ] for more info about the Naia project.
ECHO.
PAUSE
CLS