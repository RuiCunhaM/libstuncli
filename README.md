# Stun Client 

This is a very simple STUN client compatible **only** with the `XOR-MAPPED-ADDRESS` attribute described in [RFC5389](https://datatracker.ietf.org/doc/html/rfc5389).

It was design to be used in a [mptcpd plugin](https://gitlab.com/mptcp_lab/routing-plugin), therefore it allows to specify the network interface from where the Bind Request is supposed to be sent, allowing it to work in Multihoming scenarios. 

For an example on how to use it, see [example.c](example.c)



