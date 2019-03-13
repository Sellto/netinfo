package main

import (
  "fmt"
  "log"
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

func getNAT64addr(inter string) string {
  return "172.46."+strconv.Itoa(getNodeNum(inter))+".1"
}




func main() {
   var pnin string
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
