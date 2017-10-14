#include <err.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

typedef enum { CORRECT_IP, WRONG_IP, NO_IP } ip_state;

#define IFACE     argv[1]
#define WANTED_IP argv[2]
#define COMPORT   argv[3]

static char *get_ip(const char *iface);
int open_port(const char *dev);
static void sighandler(const int signo);

static unsigned short int done;

static char *
get_ip(const char *iface)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        fprintf(stderr, "get_ip: failed to get IP address for interface %s", iface);
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
        if (!strcmp(ifa->ifa_name, iface) && (ifa->ifa_addr->sa_family == AF_INET)) {
            if (s != 0) {
                fprintf(stderr, "get_ip: failed to get IP address for interface %s", iface);
                return NULL;
            }
            freeifaddrs(ifaddr);

            static char ret_str[60];
            sprintf(ret_str, "%s", host);
            return ret_str;
        }
    }

    freeifaddrs(ifaddr);
    return NULL;
}

int
open_port(const char *dev)
{
    int fd;
    
    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        fprintf(stderr, "open_port: unable to open %s", dev);
    } else {
        fcntl(fd, F_SETFL, 0);
    }

    return (fd);
}

static void
sighandler(const int signo)
{
    if (signo == SIGTERM || signo == SIGINT) {
        done = 1;
    }
}

int
main(int argc, char *argv[])
{
    if (argc == 2 && !strcmp("-v", argv[1])) {
        printf("ipcheck-%s\n", VERSION);
        exit(0);
    } else if (argc < 4 || argc > 5) {
        fputs("usage: ipcheck [iface] [wanted ip] [comport] (--no-daemon)\n"
                , stderr);
        exit(1);
    } else if (argc == 4 || strcmp("--no-daemon", argv[4])) {
        if (daemon(1, 1) < 0) {
            err(1, "daemon");
        }
    }

    /* open and configure 
     * for writing to serial port */
    struct termios options;
    cfsetospeed(&options, B9600);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    int fd = open_port(COMPORT);
    tcsetattr(fd, TCSANOW, &options);

    /* init signal handling */
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sighandler;
    sigaction(SIGINT,  &act, 0);
    sigaction(SIGTERM, &act, 0);

    /* main loop, check ip and
     * send result to arduino every second */
    int n;
    char cur_ip[60];
    ip_state state;
    while (!done) {
        if (!get_ip(IFACE)) {
            state = NO_IP;
        } else {
            strcpy(cur_ip, get_ip(IFACE));
            state = !strcmp(cur_ip, WANTED_IP) ? CORRECT_IP : WRONG_IP;
        }

        switch (state) {
            case CORRECT_IP:
                n = write(fd, "0\r", 2);
                break;
            case WRONG_IP:
                n = write(fd, "1\r", 2);
                break;
            case NO_IP:
                n = write(fd, "2\r", 2);
                break;
        }
        if (n < 0) {
            fputs("write() of 2 bytes failed!\n", stderr);
        }

        sleep(1);
    }
    return 0;
}
