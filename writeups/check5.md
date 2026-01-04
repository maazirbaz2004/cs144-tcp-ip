Checkpoint 5 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [8] hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the NetworkInterface [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:
[
Used a std::map as an ARP cache to store IP–Ethernet mappings along with TTLs.
Also maintained a std::list of waiting datagrams that could not be sent until
ARP resolution completed. Each entry had its own timeout to prevent buildup.
When sending, if the MAC address for the next hop was known, I immediately built
and transmitted an ethernet frame. Otherwise, an ARP request was broadcast.
The ARP request logic included a small timer (5 seconds) to avoid spamming
duplicate requests before the timeout expired.
Upon receiving ARP messages, the interface learns mappings from both requests
and replies, updating the cache even if it already had an entry.
When an ARP reply is received, any queued datagrams waiting for that IP are
immediately sent out with the resolved Ethernet address.
The tick function handles all time-related cleanup, reducing TTLs,
removing expired cache entries, resetting ARP timers, and dropping stale datagrams.
Considered using an unordered_map for faster lookups, but map was simpler and
avoided edge-case iterator invalidation issues during erasure.
]

Implementation Challenges:
[Understanding when to send ARP requests vs. when to reuse cached entries was tricky.
Initially, I sent ARP requests too frequently because I forgot to enforce the 5s wait.
Managing three different timers (cache TTL, request timeout, and datagram TTL)
was a bit confusing at first, but breaking them into separate loops helped.
Another challenge was ensuring broadcast frames were properly ignored or replied to
only when they were relevant to our interface’s IP.
I had to be careful when learning ARP mappings to avoid overwriting valid entries
with stale ones from outdated ARP requests.
Once I handled TTL decrements properly and made the ARP reply logic symmetrical,
the implementation became much more stable and passed all tests.]

Remaining Bugs:
[none, all tests passed]

- Optional: I had unexpected difficulty with: [nthn]

- Optional: I think you could make this lab better by: [nthn]

- Optional: I was surprised by: [nthn]

- Optional: I'm not sure about: [nthn]
