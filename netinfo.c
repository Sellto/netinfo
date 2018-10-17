
       #include <arpa/inet.h>
       #include <ifaddrs.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <string.h>
       #include <regex.h>

       #define MAX_LENGTH 128
       #define DEFAULT_IP6_PREF_LGT 64

       char *getIPV6Router(char *addr);
       char *getIPV4Router(char *addr);
       void createTaygaConfFile();
       void taygaConf(char *ipv6addr, char *ipv4addr);

       int main(int argc, char *argv[]){
           struct ifaddrs *ifaddr, *ifa;
           int err , match;
           char addr[MAX_LENGTH], net[MAX_LENGTH];
           char *name,*ipv4addr,*ipv6addr;
           regex_t preg;

           if (getifaddrs(&ifaddr) == -1) {
               perror("getifaddrs");
               exit(EXIT_FAILURE);
           }

           for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
              if (ifa->ifa_addr == NULL)
                   continue;
              if(ifa->ifa_addr->sa_family == AF_INET6)
              {
                inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr), addr, MAX_LENGTH);
                err = regcomp (&preg, "(^fe80)|(^[:]{2})", REG_NOSUB | REG_EXTENDED);
                if (err == 0)
                {
                  match = regexec (&preg, addr, 0, NULL, 0);
                  regfree (&preg);
                  if (match == REG_NOMATCH)
                  {
                    ipv6addr = malloc(strlen(addr) + 1);
                    strcpy(ipv6addr,addr);
                    name = malloc(strlen(ifa->ifa_name) + 1);
                    strcpy(name,ifa->ifa_name);
                  }
                }
              }
            }

            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
               if (ifa->ifa_addr == NULL)
                    continue;
               if(ifa->ifa_addr->sa_family == AF_INET && !(strcmp(ifa->ifa_name,name)))
               {
                 inet_ntop(AF_INET, &(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr), addr, MAX_LENGTH);
                 ipv4addr = malloc(strlen(addr) + 1);
                 strcpy(ipv4addr,addr);
               }
            }

            if(argc==1)
            {
              printf("Available commands:\n\n");
              printf("--intname\t\t\t\tused interface name\n");
              printf("--ipv4addr\t\t\t\tIPV4 address\n");
              printf("--ipv4router\t\t\t\tIPV4 router address\n");
              printf("--ipv6addr\t\t\t\tIPV6 address\n");
              printf("--ipv6router\t\t\t\tIPV6 router address\n");
              printf("--taygaconf\t\t\t\tcreate tayga.conf file\n");
              printf("--launchautoconf \t\t\tlaunch tayga with autoconf\n\n");
            }
            else if (!(strcmp(argv[1],"--ipv6addr")))
            {
              printf("%s\n",ipv6addr);
            }
            else if (!(strcmp(argv[1],"--ipv4addr")))
            {
              printf("%s\n",ipv4addr);
            }
            else if (!(strcmp(argv[1],"--intname")))
            {
              printf("%s\n",name);
            }
            else if (!(strcmp(argv[1],"--ipv6router")))
            {
              printf("%s\n",getIPV6Router(ipv6addr));
            }
            else if (!(strcmp(argv[1],"--ipv4router")))
            {
              printf("%s\n",getIPV4Router(ipv4addr));
            }
            else if (!(strcmp(argv[1],"--taygaconf")))
            {
              createTaygaConfFile();
            }
            else if (!(strcmp(argv[1],"--launchautoconf")))
            {
              taygaConf(ipv6addr,ipv4addr);
            }
           freeifaddrs(ifaddr);
           exit(EXIT_SUCCESS);
       }


       char *getIPV6Router(char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(MAX_LENGTH);
         collapse_addr=malloc(MAX_LENGTH);
         inet_pton(AF_INET6, addr, expanse_addr);
         expanse_addr[14]=0x00;
         expanse_addr[15]=0x01;
         inet_ntop(AF_INET6, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         return save;
       }

       char *getIPV4Router(char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(MAX_LENGTH/2);
         collapse_addr=malloc(MAX_LENGTH/2);
         inet_pton(AF_INET, addr, expanse_addr);
         expanse_addr[2]=0x00;
         expanse_addr[3]=0x01;
         inet_ntop(AF_INET, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         return save;
       }

       void createTaygaConfFile()
       {

         FILE *taygaconf;
         taygaconf = fopen("tayga.conf","w");
         fprintf(taygaconf,"tun-device nat64\n");
         fprintf(taygaconf,"ipv4-addr 46.0.0.1\n");
         fprintf(taygaconf,"prefix 6400::/96\n");
         fprintf(taygaconf,"dynamic-pool 46.0.0.0/8\n");
         fprintf(taygaconf,"data-dir /var/db/tayga\n");
         fclose(taygaconf);
      }

      void taygaConf(char *ipv6addr, char *ipv4addr)
      {
         char str[128];
         FILE *routing;
         routing = fopen("routing.sh","w");
         fprintf(routing,"mv tayga.conf /usr/local/etc/tayga.conf\n");
         fprintf(routing,"mkdir -p /var/db/tayga\n");
         fprintf(routing,"echo 0 >  /proc/sys/net/ipv6/conf/all/disable_ipv6\n");
         fprintf(routing,"tayga --mktun\n");
         fprintf(routing,"echo 0 >  /proc/sys/net/ipv6/conf/nat64/disable_ipv6\n");
         fprintf(routing,"ip link set nat64 up\n");
         fprintf(routing,"echo 1 >  /proc/sys/net/ipv6/conf/all/forwarding\n");
         strcpy(str,"ip addr add ");
         strcat(str,getIPV4Router(ipv4addr));
         strcat(str," dev nat64\n");
         fprintf(routing,str);
         strcpy(str,"ip addr add ");
         strcat(str,getIPV6Router(ipv6addr));
         strcat(str," dev nat64\n");
         fprintf(routing,str);
         fprintf(routing,"ip route add 46.0.0.0/8 dev nat64\n");
         fprintf(routing,"ip route add 6400::/96 dev nat64\n");
         fprintf(routing,"tayga -d\n");
         fclose(routing);
         system("chmod 755 routing.sh");
         system("./routing.sh");
         system("rm routing.sh");
       }
