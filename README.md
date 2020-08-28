# Android Client Detection

## Client_Protect (do not use Server_Protect)
Client Protect includes anti-debugging techniques and check techniques for anti-reversing on Android.
The Server Protect folder exists, but we have not implemented any of these technologies.

## Java Level Techhniques (Checker, Const)
Java Level's technology has mainly implemented rooting, emulation detection, and attack tool detection. But this is not a special technology. It's a very famous fact and code.

### Check Using Command 
which su binary detection using commands

### Check Using Pm(Package Manager)
Check the list of packages installed through pm (Package Manager), it is possible to check that a particular package is installed. This allows you to detect rooting and attack tools, speed hacks, etc. that can be used as attacks and exploits.


### Check Using Build Info
We can import information from your device through Build Info. This can be detected by determining whether the information of the representative emulator, etc. with or without the emulator.

### Check Using Android.os.Debug
The debugger can be checked through the isDebuggerConnect() function.

### Check Using Installer
GetInstallerPackageAName allows you to check if the app has been downloaded from an unallowed location.


## C/C++ Level Techhniques (native-lib.cpp)
Using the characteristics of Linux, which manages various information through files, it obtains information from various specific locations and uses it for detection.

### check using /proc/pid/maps, status
/ proc / pid / maps, status, etc. can be checked the relevant processes when the application is operating. This allows the relevant process to be inferred by name whether it is a debugger or an attack tool.

### Time Check
Check the time when turning to a specific code. This will make the time difference slower than usual when the debugger is operating together, it is possible to check the presence or absence of the debugger.

### Check USING Ptrace
each process in Linux can be run in one process at the same time, APP can preempt the pit with its own ptrace method.



## Client_Prac / Server_Prac
This is a simple example. 
Client Prac is the code that implemented a simple snake game. Server_prac is a server example for login authentication in Client_Prac, which has nothing to do with using only Cleint_Prac.

