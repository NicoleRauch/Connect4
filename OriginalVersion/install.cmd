/*	install.cmd - REXX script for installation of Connect Four
*/

call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
call SysLoadFuncs
say " "
say " "
say "    |             | "
say "    |             | "
say "    |      O      | "
say "    |      X      | "
say "    |    O X O    | "
say "    |X_X_O_O_X____| "
say " "
say " "
say "Connect Four - (C) 1994 Ralf Seidel and Nicole Greiber"
say " "
say "Installation in progress - please wait ..."
say " "
havemmsound = RxFuncAdd('mciRxInit', 'MCIAPI', 'mciRxInit')
say " "
if havemmsound <> 0 then 
do
rc = SysFileTree("c4mm.EXE", fspec, "FO")
say "The Program will be installed with sound ..."
end
else 
do
rc = SysFileTree("c4nomm.EXE", fspec, "FO")
say "The program will be installed without sound ..."
end
say " "
if rc=0 then
do
	filename=filespec("Name", fspec.1)
	filepath=filespec("Path", fspec.1)
	filedrive=filespec("Drive", fspec.1)
	if SysCreateObject("WPProgram", "Connect Four",,
	  "<WP_DESKTOP>", "EXENAME="filedrive||filepath||filename,
	  ";STARTUPDIR="||filedrive||filepath, "replace") then
	do
		say "The Connect Four program object has successfully been created."
		say " "
		say "Please make sure that you have placed the emx-DLLs somewhere in your LIBPATH."
	end
	else
		say "ERROR: The program object could not be created."
end
else
	say "ERROR: Connect Four program file connect4.exe not found."
	exit
end
