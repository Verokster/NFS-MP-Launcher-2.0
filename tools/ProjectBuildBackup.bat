@ECHO OFF
IF NOT "%~1" == "/?" GOTO Main

ECHO Project Build Backup [v1.0]
ECHO Creates a compressed archive of a project.
ECHO Designed for use with Visual Studio for automated build backups.
ECHO Copyright (c) 2013, Jason Faulkner - All rights reserved.
ECHO.
ECHO %~n0 BackupToFolder ProjectPath ProjectName 
ECHO                    [/O SubfolderPath] [/T Tag] [/D] [/7Z]
ECHO.
ECHO  BackupToFolder
ECHO           Root folder which holds the build backups. The actual backup
ECHO           archive will be placed in a subfolder of this directory.
ECHO  ProjectPath
ECHO           Root path of the project to backup. This folder should contain
ECHO           all the project files which need to be backed up.
ECHO  ProjectName
ECHO           Name of the project being backed up.
ECHO  /O SubfolderPath
ECHO           Subfolder, respective to the BackupToFolder, where the backup
ECHO           should be placed. Default value is "builds".
ECHO           If this folder does not exist, it will be created.
ECHO  /T Tag   Additional text to append to the backup file name.
ECHO  /D       If provided, appends the current date/time to end of the backup
ECHO           file name. If both /T and /D are provided, /T is appended first.
ECHO  /7Z      If provided, the archive is created in .7z format instead of the
ECHO           default extension of .zip.
ECHO.
ECHO.
ECHO Backups are created in the following directory:
ECHO    ^<BackupToFolder^>\{builds ^| ^<SubfolderPath^>}
ECHO.
ECHO File name generation formula:
ECHO    ^<ProjectName^>[[_^<Tag^>]_^<TimeStamp^>].{zip ^| 7z}
ECHO.
ECHO Note that if the generated output file already exists, it will be overwritten.
ECHO It is recommended that that /D flag be used to help avoid this.
ECHO.
ECHO.
ECHO Requirements (must be in the current directory or in a PATH folder):
ECHO  7z.exe
ECHO.
ECHO __________
ECHO Visit my website for additional information, examples and updates.
ECHO http://jasonfaulkner.com
GOTO :EOF



:Main
SETLOCAL EnableExtensions EnableDelayedExpansion

CALL :Initialize
CALL :Configuration %*
IF /I %ERRORLEVEL% GTR 0 GOTO Finish
CALL :PrepSettings
IF /I %ERRORLEVEL% GTR 0 GOTO Finish


REM Ready variables ---
REM FilePath
REM FileName
REM DestFile = "FilePath\FileName"

ECHO.
ECHO Project Build Backup

ECHO Creating build backup.

ECHO File Path  : %FilePath%
ECHO File Name  : %FileName%
ECHO Destination: %DestFile%

REM Check that the output folder exist.
IF NOT EXIST "%FilePath%" (
	ECHO Target folder does not exist. Creating.
	MKDIR "%FilePath%"
)


REM Delete the destination file with the same name if it already exists.
IF EXIST %DestFile% (
	ECHO Destination file already exists. Deleting.
	DEL /F %DestFile%
)

ECHO.


SET BackupSource="%ProjectPath%\*"

REM Trim double dir separators.
SET BackupSource=%BackupSource:\\=\%

REM Run the backup.
%~dp0\7z.exe a %DestFile% %BackupSource%

REM Show the destination as the final output line.
ECHO.
ECHO Final output --
ECHO %DestFile%


:Finish
ECHO.
ENDLOCAL
GOTO :EOF




:Initialize
REM File extension to use when compressing the backup.
REM This must be recognized by the '7z' utility.
SET CompressFileExtension=zip

REM Set defaults.
SET OutputFolder=builds
SET IncludeOnly=null
SET DateAppend=null
SET Tag=null

GOTO :EOF


:Configuration
IF "%~3" == "" GOTO InvalidParams

SET SolutionPath=%~1
SET ProjectPath=%~2
SET ProjectName=%~3

:Params
IF "%~4" == "" GOTO :EOF

IF /I "%~4" == "/D" (
	REM Note, this assumes the %DATE% token returns format: Day MM/DD/YYYY
	REM Where (from the format above) %%A=Day, %%B=MM, %%C=DD, %%D=YYYY
	REM You may need to adjust accordingly for your regional settings.
	FOR /F "usebackq tokens=1,2,3,4,5,6 delims=/: " %%A IN ('%DATE% %TIME%') DO SET DateAppend=%%D-%%B-%%C_%%E%%F_%%A
)

IF /I "%~4" == "/O" (
	SET OutputFolder=%~5
	SHIFT /4
)

IF /I "%~4" == "/T" (
	SET Tag=%~5
	SHIFT /4
)

IF /I "%~4" == "/I" (
	SET IncludeOnly=%~5
	SHIFT /4
)

IF /I "%~4" == "/7Z" SET CompressFileExtension=7z

SHIFT /4
GOTO :Params


:PrepSettings
REM Put together the destination file name.
SET FilePath=%SolutionPath%\%OutputFolder%

REM Trim double dir separators.
SET FilePath=%FilePath:\\=\%


SET FileName=%ProjectName%

IF NOT "%Tag%" == "null" SET FileName=%FileName%_%Tag%
IF NOT "%DateAppend%" == "null" SET FileName=%FileName%_%DateAppend%

SET FileName=%FileName%.%CompressFileExtension%

SET DestFile="%FilePath%\%FileName%"

GOTO :EOF



:CheckRequirements
CALL :VerifyRequirement "7z.exe"
GOTO :EOF

:VerifyRequirement
IF EXIST "%~1" GOTO :EOF
IF NOT "%~$PATH:1" == "" GOTO :EOF
ECHO Missing requirement: [%~1] Use /? to view the help information.
EXIT /B 2

:InvalidParams
ECHO Invalid parameters. Use /? to view the help information.
EXIT /B 1