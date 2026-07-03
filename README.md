# MiniNet — Building the Internet from Scratch in C

This is not a tutorial. It's a ground-up implementation of the entire
network stack from raw bit transmission to cloud infrastructure
written in C, one protocol at a time,why?cuz  **I DONT WANT PEACE I WANT PROBLEMS**.

Every byte is explained. Nothing is skipped. No black boxes.

AI is only used as a tool to learn concepts I am unfamiliar with, no part of this 
is "vibe coded".

## The Idea

Most people learn networking top-down: HTTP → TCP → IP → "somehow it works."
This project goes the other direction. We start at the wire and build upward,
implementing each layer ourselves before using it.

By the end, this repository will contain working implementations of Ethernet,
ARP, IP, ICMP, UDP, TCP, DNS, DHCP, NAT, TLS, and a minimal cloud networking
stack each one built from the RFC, tested with real packet captures, and
documented as we go.

## Where This Is Going

This is not just a networking project. The endgame is Mini AWS.
Chapters 0–2    Ethernet + switching       → working LAN
Chapters 3–8    ARP, IP, routing, DNS      → working internet in miniature
Chapters 9–10   UDP + TCP                  → reliable transport
Part V          HTTP, SMTP                 → application layer
Part VI         TLS                        → your TCP gets encrypted
Part VII        Network namespaces, veth,
Linux bridges, NAT         → working VPC
Part VIII       Load balancer, service
discovery, distributed
storage                    → Mini AWS

Nothing gets thrown away. The Ethernet frame struct from Chapter 1 runs
inside the virtual interfaces in Part VII. The switch from Chapter 2 is
architecturally identical to a Linux bridge. The TCP stack from Chapter 10
is what the load balancer in Part VIII sits on top of.

Every chapter is load-bearing.

## Structure

| Chapter | Topic | Status |
|---------|-------|--------|
| 0 | Bit transmission over a Unix pipe | ✅ Done |
| 1 | Ethernet frames + CRC-32 | ✅ Done |
| 2 | Ethernet switch (learning table) | 🔨 Next |
| 3 | ARP | — |
| 4 | IPv4 | — |
| 5 | ICMP + Ping | — |
| 6 | Routing | — |
| 7 | DHCP | — |
| 8 | DNS | — |
| 9 | UDP | — |
| 10 | TCP | — |

## Rules

**No magic.** When we reach `0x0800` you'll know why it's `0x0800`,
why it's 2 bytes, how it sits in memory, and how Wireshark decodes it.

**No code dumps.** Every function in this repo was written by hand,
reviewed line by line, and tested before moving on.

**No skipping.** Each chapter builds on the last. There are no shortcuts.

## Environment

- Language: C (C11)
- Platform: Linux (Ubuntu Server)
- Tools: gcc, tcpdump, Wireshark, Python3 for verification

## Why

Because understanding only goes as deep as what you've built yourself.
