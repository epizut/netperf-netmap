#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif

#include <sys/types.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <linux/sockios.h>

/* alas, direct inclusion of ethtool.h depends on some types not
   normally found in nature, which we must provide or things will be
   quite unhappy. */

typedef __uint64_t __u64;
typedef __uint32_t __u32;
typedef __uint16_t __u16;
typedef __uint8_t  __u8;

/* older ones want them without the leading underscores */
typedef __uint64_t u64;
typedef __uint32_t u32;
typedef __uint16_t u16;
typedef __uint8_t  u8;

#include <linux/ethtool.h>

void
find_driver_info(char *ifname, char *driver, char *version, char *firmware, char *bus, int len) {

  int s;
  int ret;
  struct ifreq ifr;
  struct ethtool_drvinfo drvinfo;

  if (len < 32) return;

  if (!strcmp(ifname,"lo")) {
    /* special case loopback */
    strncpy(driver,"loopback",len);
    strncpy(version,"system",len);
    strncpy(firmware,"N/A",len);
    strncpy(bus,"N/A",len);
    driver[len-1] = 0;
    version[len-1] = 0;
    firmware[len-1] = 0;
    bus[len-1] = 0;
    return;
  }

  s = socket(AF_INET,SOCK_DGRAM,0);

  if (s < 0) {
    strncpy(driver,"SocketFailure",len);
    strncpy(version,"SocketFailure",len);
    strncpy(firmware,"SocketFailure",len);
    strncpy(bus,"SocketFailure",len);
    driver[len-1] = 0;
    version[len-1] = 0;
    firmware[len-1] = 0;
    bus[len-1] = 0;
    return;
  }

  memset(&ifr, 0, sizeof(ifr));
  drvinfo.cmd = ETHTOOL_GDRVINFO;
  strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name)-1);
  ifr.ifr_data = (caddr_t)&drvinfo;

  ret = ioctl(s, SIOCETHTOOL, &ifr);

  if (ret == -1) {
    strncpy(driver,"IoctlFailure",len);
    strncpy(version,"IoctlFailure",len);
    strncpy(firmware,"IoctlFailure",len);
    strncpy(bus,"IoctlFailure",len);
    driver[len-1] = 0;
    version[len-1] = 0;
    firmware[len-1] = 0;
    bus[len-1] = 0;
    return;
  }
  strncpy(driver,drvinfo.driver,len);
  strncpy(version,drvinfo.version,len);
  strncpy(firmware,drvinfo.fw_version,len);
  strncpy(bus,drvinfo.bus_info,len);
  driver[len-1] = 0;
  version[len-1] = 0;
  firmware[len-1] = 0;
  bus[len-1] = 0;
  
  return;
}

#if defined(NETPERF_STANDALONE_DEBUG)
int
main(int argc, char *argv[]) {

  char driver[32];
  char version[32];
  char firmware[32];
  char businfo[32];

  if (argc != 2) {
    fprintf(stderr,"%s <interface>\n",argv[0]);
    return -1;
  }

  find_driver_info(argv[1],driver, version, firmware, businfo, 32);

  printf("For %s driver %s version %s firmware %s businfo %s\n",
	 argv[1],driver, version, firmware, businfo);

  return 0;
}
#endif
