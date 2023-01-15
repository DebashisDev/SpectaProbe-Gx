#include <signal.h>

#include "SpectaProbe.h"

void sig_handler(int signo)
{
	if (signo == SIGTERM || signo == SIGINT)
	{
		printf("\n Probe Shutdown Initiated....\n");
		IPGlobal::PROBE_RUNNING_STATUS = false;
		IPGlobal::PACKET_PROCESSING = false;
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("USAGE:: spectaProbe <<Config File Name>>\n");
		exit(1);
	}

	sleep(5);

	/* Initialize all the Locks */
	mapGxLock::count		= 1;

	if (signal(SIGTERM, sig_handler) == SIG_ERR || signal(SIGINT, sig_handler) == SIG_ERR)
		printf("SpectaProbe Can't Received Signal...\n");

	timeval curTime;
	struct tm *now_tm;

	IPGlobal::PROBE_RUNNING_STATUS = true;
	SpectaProbe *spectaProbe = new SpectaProbe(argv[1]);
	spectaProbe->start();

	printf("  **** SpectaProbe Exiting...Please wait... ***** \n");
}
