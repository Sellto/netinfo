
       #include <arpa/inet.h>
       #include <ifaddrs.h>
       #include <stdio.h>
       #include <stdlib.h>
       #include <string.h>
       #include <regex.h>

       #define MAX_LENGTH 128
       #define DEFAULT_IP6_PREF_LGT 64




       char *getIPV6NatSubnet(int net_lenght, char *addr);
       char *getIPV6Prefix(int Lenght, char *addr);
       char *getIPV6Router(char *addr);
       char *getIPV4Router(char *addr);
       char *getNATIPV4net(char *addr);
       void createTaygaConfFile(char *pooladdr,char *netsize, char *ipv6addr, char *ipv4addr);

       int main(int argc, char *argv[]){
           struct ifaddrs *ifaddr, *ifa;
           int err , match;
           char addr[MAX_LENGTH], net[MAX_LENGTH];
           char *ipv4name,*ipv4addr,*ipv6name,*ipv6addr;
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
                    ipv6name = malloc(strlen(ifa->ifa_name) + 1);
                    strcpy(ipv6name,ifa->ifa_name);
                  }
                }
              }
            }

            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
               if (ifa->ifa_addr == NULL)
                    continue;
               if(ifa->ifa_addr->sa_family == AF_INET && strcmp(ifa->ifa_name,ipv6name))
               {
                 inet_ntop(AF_INET, &(((struct sockaddr_in *)ifa->ifa_addr)->sin_addr), addr, MAX_LENGTH);
                 ipv4addr = malloc(strlen(addr) + 1);
                 strcpy(ipv4addr,addr);
                 ipv4name = malloc(strlen(ifa->ifa_name) + 1);
                 strcpy(ipv4name,ifa->ifa_name);
               }
            }

            if(argc==1)
            {
              printf("Available commands:\n\n");
              printf("--ipv4name\t\t\t\tIPV4 interface name\n");
              printf("--ipv4addr\t\t\t\tIPV4 address\n");
              printf("--ipv4router\t\t\t\tIPV4 router address\n\n");
              printf("--nat64IPV4netr\t\t\t\tIPV4 nat64 network\n\n");
              printf("--ipv6addr\t\t\t\tIPV6 address\n");
              printf("--ipv6name\t\t\t\tIPV6 interface name\n");
              printf("--ipv6natsubnet\t\t\t\tIPV6 nat subnet\n");
              printf("--ipv6router\t\t\t\tIPV6 router address\n");
              printf("--ipv6net\t\t\t\tIPV6 prefix\n\n");
            }
            else if (!(strcmp(argv[1],"--ipv6addr")))
            {
              printf("%s\n",ipv6addr);
            }
            else if (!(strcmp(argv[1],"--ipv4addr")))
            {
              printf("%s\n",ipv4addr);
            }
            else if (!(strcmp(argv[1],"--ipv6name")))
            {
              printf("%s\n",ipv6name);
            }
            else if (!(strcmp(argv[1],"--ipv4name")))
            {
              printf("%s\n",ipv4name);
            }
            else if (!(strcmp(argv[1],"--ipv6natsubnet")))
            {
              printf("%s\n",getIPV6NatSubnet(96,ipv6addr));
            }
            else if (!(strcmp(argv[1],"--ipv6router")))
            {
              printf("%s\n",getIPV6Router(ipv6addr));
            }
            else if (!(strcmp(argv[1],"--ipv4router")))
            {
              printf("%s\n",getIPV4Router(ipv4addr));
            }
            else if (!(strcmp(argv[1],"--nat64IPV4net")))
            {
              printf("%s\n",getNATIPV4net(ipv4addr));
            }
            else if (!(strcmp(argv[1],"--ipv6net")))
            {
              printf("%s\n",getIPV6Prefix(DEFAULT_IP6_PREF_LGT,ipv6addr));
            }
           freeifaddrs(ifaddr);
           exit(EXIT_SUCCESS);
       }

       char *getIPV6NatSubnet(int net_lenght, char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(net_lenght);
         collapse_addr=malloc(net_lenght);
         inet_pton(AF_INET6, addr, expanse_addr);
         expanse_addr[net_lenght/8-2]=0x00;
         expanse_addr[net_lenght/8-1]=0x04;
         for (int i = net_lenght/8;i < 16; i++)
         {
             expanse_addr[i] = 0x00;
         }
         inet_ntop(AF_INET6, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         return save;
       }

       char *getIPV6Prefix(int net_lenght, char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(net_lenght);
         collapse_addr=malloc(net_lenght);
         inet_pton(AF_INET6, addr, expanse_addr);
         for (int i = net_lenght/8;i < 16; i++)
         {
             expanse_addr[i] = 0x00;
         }
         inet_ntop(AF_INET6, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         return save;
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

       // char *getIPV4Router(char *addr)
       // {
       //   char *expanse_addr,*collapse_addr,*save;
       //   expanse_addr=malloc(MAX_LENGTH/2);
       //   collapse_addr=malloc(MAX_LENGTH/2);
       //   inet_pton(AF_INET, addr, expanse_addr);
       //   expanse_addr[2]=0x00;
       //   expanse_addr[3]=0x01;
       //   inet_ntop(AF_INET, expanse_addr,collapse_addr, MAX_LENGTH);
       //   save = malloc(strlen(collapse_addr) + 1);
       //   strcpy(save,collapse_addr);
       //   return save;
       // }

       char *getIPV4Router(char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(MAX_LENGTH/2);
         collapse_addr=malloc(MAX_LENGTH/2);
         inet_pton(AF_INET, addr, expanse_addr);
         expanse_addr[0]=0xAC;
         expanse_addr[1]=0x2E;
         //expanse_addr[2]=0x00;
         expanse_addr[3]=0x01;
         inet_ntop(AF_INET, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         return save;
       }

       char *getNATIPV4net(char *addr)
       {
         char *expanse_addr,*collapse_addr,*save;
         expanse_addr=malloc(MAX_LENGTH/2);
         collapse_addr=malloc(MAX_LENGTH/2);
         inet_pton(AF_INET, addr, expanse_addr);
         expanse_addr[0]=0xAC;
         expanse_addr[1]=0x2E;
         //expanse_addr[2]=0x00;
         expanse_addr[3]=0x00;
         inet_ntop(AF_INET, expanse_addr,collapse_addr, MAX_LENGTH);
         save = malloc(strlen(collapse_addr) + 1);
         strcpy(save,collapse_addr);
         strcat(save,"/24");
         return save;
       }
