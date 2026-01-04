Checkpoint 7 Writeup
====================

My name: [Mohib]

My SUNet ID: [23L-0650]

I collaborated with: [23L-0728]

I would like to thank/reward these classmates for their help: [23L-0811]

This checkpoint took me about [0.5] hour to do. I [did/did not] attend the lab session.

Solo portion:

- Did your implementation successfully start and end a conversation with another copy of itself? [YES]

- Did it successfully transfer a one-megabyte file, with contents identical upon receipt? [y]

- Please describe what code changes, if any, were necessary to pass these steps: nope, none were needed. 

Group portion:

- Who is your lab partner (and what is their SUNet ID, e.g. "winstein"?): Collaborated with Abdullah Iqbal (23L-0811)

- Did your implementations successfully start and end a conversation with each other (with each implementation acting as ``client'' or as ``server'')?
Ans -> Yes. We tested both directions (each acting as client and server), and it worked correctly.

- Did you successfully transfer a one-megabyte file between your two
  implementations, with contents identical upon receipt?
Ans -> Yes. Both implementations produced matching SHA-256 hashes as shown below, confirming the file transferred perfectly.

Abdullah's output:
[abdullah-iqbal@abdullah-iqbal-VirtualBox:~/l230811/CnAssignment/build$ sha256sum /tmp/big-received.txt
c9a2375f39b9e1155aff1fbfa15d708900dbbf70639d04666b3da4694f8fe84a  /tmp/big-received.txt]

My output:
mohib@mohib-VMware-Virtual-Platform:~/minnow-Stanford/build$ sha256sum /tmp/big.txt
c9a2375f39b9e1155aff1fbfa15d708900dbbf70639d04666b3da4694f8fe84a  /tmp/big.txt

- Please describe what code changes, if any, were necessary to pass
  these steps, either by you or your lab partner.
Ans -> nope, none were needed!

Creative portion (if you did anything for our creative challenge,
                  please boast about it!)

- Optional: I had unexpected difficulty with: [nthn]

- Optional: I think you could make this lab better by: [nthn]

- Optional: I was surprised by: [nthn]

- Optional: I'm not sure about: [nthn]
