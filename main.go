package main

import (
	"log"

	"github.com/jsimonetti/rtnetlink"
	"github.com/mdlayher/netlink"
	"golang.org/x/sys/unix"
)

// List all interfaces
func main() {
	// Dial a connection to the rtnetlink socket
	conn, err := rtnetlink.Dial(&netlink.Config{
		Groups:                unix.RTNLGRP_LINK | unix.RTNLGRP_NSID,
		EnableControlMessages: true,
	})
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	conn.SetOption(netlink.ListenAllNSID, true)

	// Request a list of interfaces
	for {
		// log.Println("Waiting for messages")
		msgs, nlMsgs, err := conn.Receive()
		if err != nil {
			log.Fatal(err)
		}
		for i, msg := range msgs {
			if nlMsgs[i].Header.Type == unix.RTM_NEWLINK {
				if _, ok := msg.(*rtnetlink.LinkMessage); ok {
					link := msg.(*rtnetlink.LinkMessage)
					var netnsid int
					if len(nlMsgs[i].OobData) > 0 {
						cmsg, err := unix.ParseSocketControlMessage(nlMsgs[i].OobData)
						if err != nil {
							log.Printf("Error parsing oob data: %v", err)
							continue
						}
						for _, oob := range cmsg {
							if oob.Header.Level == unix.SOL_NETLINK && oob.Header.Type == unix.NETLINK_LISTEN_ALL_NSID {
								netnsid = int(oob.Data[0])
							}
							log.Printf("(%d) %s added to netns %d\n", link.Index, link.Attributes.Name, netnsid)
							break
						}
					}
				}
			}
		}
	}

}
