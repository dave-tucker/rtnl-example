#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

int main() {
  int fd;
  struct sockaddr_nl sa;
  memset(&sa, 0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_groups = RTNLGRP_LINK | RTNLGRP_NSID;
  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);

  setsockopt(fd, SOL_NETLINK, NETLINK_LISTEN_ALL_NSID, &(int){1}, sizeof(int));
  bind(fd, (struct sockaddr *)&sa, sizeof(sa));
  while (1) {
    char buf[4096];
    char oobbuf[4096];
    struct msghdr msg;
    struct iovec iov;
    struct sockaddr_nl nladdr = {.nl_family = AF_NETLINK};

    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    msg.msg_name = &nladdr;
    msg.msg_namelen = sizeof(sa);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = oobbuf;
    msg.msg_controllen = sizeof(oobbuf);

    int len = recvmsg(fd, &msg, 0);
    struct nlmsghdr *nh;
    int netnsid;
    struct cmsghdr *cmsg;
    printf("Received %d bytes and %zu control bytes\n", len,
           msg.msg_controllen);
    cmsg = CMSG_FIRSTHDR(&msg);
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg))
      if (cmsg->cmsg_level == SOL_NETLINK &&
          cmsg->cmsg_type == NETLINK_LISTEN_ALL_NSID &&
          cmsg->cmsg_len == CMSG_LEN(sizeof(int))) {
        int *data = (int *)CMSG_DATA(cmsg);
        printf("Received netnsid %d\n", *data);
        netnsid = *data;
      }
    for (nh = (struct nlmsghdr *)buf; NLMSG_OK(nh, len);
         nh = NLMSG_NEXT(nh, len)) {

      if (nh->nlmsg_type == NLMSG_DONE) {
        break;
      } else if (nh->nlmsg_type == NLMSG_ERROR) {
        printf("Error in netlink\n");
        break;
      } else if (nh->nlmsg_type == RTM_NEWLINK) {
        printf("New link in ns %d\n", netnsid);
      } else if (nh->nlmsg_type == RTM_DELLINK) {
        printf("Delete link in ns %d\n", netnsid);
      } else if (nh->nlmsg_type == RTM_NEWNSID) {
        printf("New netnsid %d\n", netnsid);
      } else if (nh->nlmsg_type == RTM_DELNSID) {
        printf("Delete netnsid %d\n", netnsid);
      }
    }
  }
}
