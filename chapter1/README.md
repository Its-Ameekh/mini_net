# Chapter 1 — Ethernet

You can read my code comments to understand the logic better 

## The Problem 

Imagine ten computers plugged into the same cable. When one sends bits,
every other machine sees them. How does the right machine know a message
is for it? How does anyone know where one message ends and the next begins?
How do you detect if the cable corrupted something in transit?

Ethernet solves all three.

## What We Built

A complete Ethernet II frame the envelope that wraps every packet on
a local network from scratch in C.

## The Frame

Every Ethernet frame is structured like this:

| Field           | Size    | Purpose                                      |
|-----------------|---------|----------------------------------------------|
| Destination MAC | 6 bytes | Who this frame is for                        |
| Source MAC      | 6 bytes | Who sent it                                  |
| EtherType       | 2 bytes | What's inside (0x0800 = IPv4, 0x0806 = ARP)  |
| Payload         | 46–1500 | The actual data                              |
| CRC-32          | 4 bytes | Corruption check                             |

MAC addresses are 48-bit identifiers burned into every NIC at the factory.
The first 3 bytes identify the manufacturer. The last 3 identify the device.
`ff:ff:ff:ff:ff:ff` is broadcast every machine on the segment receives it.

Minimum payload is 46 bytes because old shared-medium Ethernet needed frames
to stay on the wire long enough for collision detection to work across the
full cable length. We zero-pad anything shorter.

## Why `__attribute__((packed))` Is Not Optional

The C compiler normally inserts invisible padding bytes between struct fields
to align them to word boundaries. For a regular struct this is fine — it makes
memory access faster. For a network frame it is a correctness bug.

Without `packed`, our struct is 1520 bytes instead of 1518. The compiler
inserted 2 padding bytes before the CRC field to align it to a 4-byte boundary.
If you cast a raw packet into an unpacked struct, every byte from that padding
point onward maps to the wrong field. The CRC you read is not the CRC that
was sent. The frame appears corrupted even when it isn't.

`packed` forces the struct layout to match the wire layout exactly.
It is not a style choice — it is what makes the cast safe.

## Why `payload_len` Lives Outside the Struct

The struct is a wire format. Every byte in it has a position that real NICs,
switches, and Wireshark all agree on. Adding a bookkeeping field like
`payload_len` would change `sizeof(ethernet_frame)` and break every cast,
every serialization, every comparison against real captured packets.

The struct is sacred. Metadata about the struct lives outside it.

## CRC-32

The CRC is computed by treating the frame bytes as a giant number and dividing
by the polynomial `0x04C11DB7` (stored in reflected form as `0xEDB88320`).
The 4-byte remainder is appended to the frame. The receiver runs the same
computation — if a single bit flipped in transit, the remainder changes and
the frame is silently dropped.

We verified our implementation against Python's `binascii.crc32`. Both sides
produced `0x77773f85` for the same input.

## Files

- `ethernet.h` — packed struct definition and function prototypes
- `ethernet.c` — build_frame, serialize_frame, parse_frame, CRC-32
- `main.c`     — round-trip test: build → serialize → parse → verify
