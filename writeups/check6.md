Checkpoint 6 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [4] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]: 
[
Used a vector to store route entries in the routing table.
Each route entry keeps the prefix, prefix length, optional next hop,
and the interface number it should be forwarded to.
When a datagram is received, the router performs a longest prefix match
by iterating over all routes and comparing masked IP prefixes.
A 32 bit mask is generated using bit shifting to match the
prefix length. The route with the largest matching prefix is selected.
The route() function goes through all network interfaces, retrieves
queued datagrams and decides where each one should be sent next.
If no route matches, the datagram is dropped quietly.
If TTL reaches zero after decrementing, the datagram is also dropped.
Considered using a trie structure for routing, but a vector loop was
simpler better to use.
]

Implementation Challenges:
[The biggest issue was handling the ttl correctly when forwarding datagrams.
Initially, the datagram was dropped with the error "bad IPv4 datagram".
The problem was that after decrementing ttl, I forgot to recompute the
header checksum, which made the packet invalid.
The fix was to call "dgram.header.compute_checksum()" right after
decreasing ttl, ensuring a valid header before forwarding.
Understanding the longest-prefix-match logic took some trial and error.
At first, I incorrectly applied the subnet mask, but then I learned
to shift it properly.
Another challenge was determining when to use the next hop and when to 
use the direct destination address.
]

Remaining Bugs:
[none]

- Optional: I had unexpected difficulty with: [nthn]

- Optional: I think you could make this lab better by: [nthn]

- Optional: I was surprised by: [nthn]

- Optional: I'm not sure about: [nthn]
