package main

import (
	"log"

	"github.com/jsimonetti/rtnetlink"
	"github.com/mdlayher/netlink"
)

// List all interfaces
func main() {
	// Dial a connection to the rtnetlink socket
	conn, err := rtnetlink.Dial(&netlink.Config{
		Groups: 0x1,
	})
	if err != nil {
		log.Fatal(err)
	}
	defer conn.Close()
	conn.SetOption(netlink.ListenAllNSID, true)

	// Request a list of interfaces
	for {
		log.Println("Waiting for messages")
		msgs, nlMsgs, err := conn.Receive()
		if err != nil {
			log.Fatal(err)
		}
		for i, msg := range msgs {
			if _, ok := msg.(*rtnetlink.LinkMessage); ok {
				if nlMsgs[i].Header.Type == 0x10 {
					link := msg.(*rtnetlink.LinkMessage)
					if link.Change == 0 {
						continue
					}
					log.Printf("(%d) %s", link.Index, link.Attributes.Name)
					log.Printf("%#v", link)
				}
			}
		}
	}

}
