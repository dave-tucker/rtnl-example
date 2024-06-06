module github.com/dave-tucker/rtnl-example

go 1.21.3

replace github.com/mdlayher/netlink => github.com/dave-tucker/go-netlink v0.0.0-20240312110414-5cfa69a3d38e

require (
	github.com/jsimonetti/rtnetlink v1.4.1
	github.com/mdlayher/netlink v1.7.2
	golang.org/x/sys v0.18.0
)

require (
	github.com/google/go-cmp v0.6.0 // indirect
	github.com/josharian/native v1.1.0 // indirect
	github.com/mdlayher/socket v0.5.0 // indirect
	golang.org/x/net v0.22.0 // indirect
	golang.org/x/sync v0.6.0 // indirect
)
