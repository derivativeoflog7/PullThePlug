# PullThePlug
(Almost) immediately shut off the computer, (almost) like if someone pulled the power plug.  
This is accomplished by calling two functions inside NTDLL.DLL that are normally ran at the end of the normal shutdown process, [NtShutdownSystem](http://undocumented.ntinternals.net/index.html?page=UserMode%2FUndocumented%20Functions%2FHardware%2FNtShutdownSystem.html) and NtSetSystemPowerState.  

# Warning!  
Running this program will immediately shut down your computer, all unsaved data will be lost.  
Loss of data or data corruption could also be caused by the sudden shutdown (just like it could if you were to unplug your PC from power!)  
I'm not responsable for any loss of data caused by the usage of this program.  

# Usage  
Just open the executable.  
Precompiled builds support Windows XP and onwards.  

Most of the original work done by [hxhl95](https://www.codeproject.com/Articles/34194/Performing-emergency-shutdowns), I just reimplemented part of their code.

