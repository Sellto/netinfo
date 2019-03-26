package main

import (
  "fmt"
  "log"
  "net"
  "github.com/hashicorp/go-sockaddr"
  "strconv"
  "os"
  "github.com/urfave/cli"
)

func getNodeNum(inter string) int {
  ip,_ := sockaddr.GetInterfaceIP(inter)
  ipv4,_ := sockaddr.NewIPv4Addr(ip)
  return ipv4.Octets()[2]
}

func getNAT64pool(inter string) string {
  return "172.46."+strconv.Itoa(getNodeNum(inter))+".0/24"
}

func getDNSserver(dns string,prefix string) string {
  ips, err := net.LookupIP(dns)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Could not get IPs: %v\n", err)
		os.Exit(1)
	}
  ipv4,_ := sockaddr.NewIPv6Addr(prefix+ips[0].String())
	return ipv4.String()


}

func getNAT64addr(inter string) string {
  return "172.46."+strconv.Itoa(getNodeNum(inter))+".1"
}




func main() {
   var pnin string
   var dns string
   var prefix string
    app := cli.NewApp()
    app.Name = "Netinfo"
    app.Usage= "Program used to get the tayga pool IPv4 address based on the pod address gived by flannel"
    app.Email = "selleslagh.tom@gmail.com"
    app.Author = "Selleslagh Tom"
    app.Version = "1.0"
    app.Commands = []cli.Command{
       {
         Name:    "nodenum",
         Aliases: []string{"n"},
         Usage:   "Get the Node number based on the third number of the IPv4 addr",
         Action:  func(c *cli.Context) error {
           fmt.Println(getNodeNum(pnin))
           return nil
         },
         Flags: []cli.Flag {
           cli.StringFlag{
            Name: "interface, i",
            Value: "eth0",
            Usage: "physical network interface name",
            Destination: &pnin,
          },
        },
       },
       {
         Name:    "translate",
         Aliases: []string{"d"},
         Usage:   "translate the dns ipv4 to tayga ipv6 address",
         Action:  func(c *cli.Context) error {
           fmt.Println(getDNSserver(dns,prefix))
           return nil
         },
         Flags: []cli.Flag {
           cli.StringFlag{
            Name: "dns",
            Usage: "dns value to transform",
            Destination: &dns,
          },
          cli.StringFlag{
           Name: "prefix",
           Usage: "prefix ipv6 to put in front of the ipv4 address",
           Destination: &prefix,
         },
        },
       },
       {
         Name:    "poolnet",
         Aliases: []string{"p"},
         Usage:   "Get the /24 pool network ",
         Action:  func(c *cli.Context) error {
           fmt.Println(getNAT64pool(pnin))
           return nil
         },
         Flags: []cli.Flag {
           cli.StringFlag{
            Name: "interface, i",
            Value: "eth0",
            Usage: "physical network interface name",
            Destination: &pnin,
          },
        },
       },
       {
         Name:    "ipv4one",
         Aliases: []string{"r"},
         Usage:   "Get the router address",
         Action:  func(c *cli.Context) error {
           fmt.Println(getNAT64addr(pnin))
           return nil
         },
         Flags: []cli.Flag {
           cli.StringFlag{
            Name: "interface, i",
            Value: "eth0",
            Usage: "physical network interface name",
            Destination: &pnin,
          },
        },
       },
     }

   err := app.Run(os.Args)
   if err != nil {
     log.Fatal(err)
   }
 }
