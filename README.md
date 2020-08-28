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



## Client_Prac / Server_Prac
This is a simple example. 
Client Prac is the code that implemented a simple snake game. Server_prac is a server example for login authentication in Client_Prac, which has nothing to do with using only Cleint_Prac.

