# Chapter 2 — The Ethernet Switch

## The Problem

Chapter 1 gave us Ethernet frames — envelopes with a destination MAC address.
But knowing *who* a frame is addressed to is useless if you don't know *where*
to send it.

The naive solution is flooding — when a frame arrives, send it out every port
and let every machine decide if it's theirs. It works, but it's wasteful.
A 48-port switch flooding every frame means every packet goes to 47 machines
that don't want it. That's 98% wasted bandwidth at scale.

A real switch fixes this by *learning* — it watches incoming frames and builds
a map of which MAC address lives on which port. Once it knows, it forwards
only to the right port.

## How It Works

### Learning

When a frame arrives on port N, the switch reads the *source* MAC and records:
AA:BB:CC:DD:EE:FF → port N, last seen: now

This happens for free on every frame. No configuration. No protocol.
The switch learns the layout of the network just by watching traffic.

### Forwarding

When a frame arrives destined for a known MAC, the switch looks it up
and sends it out only that port. One port. Not all of them.

### Flooding

When the destination MAC is unknown — or when it's the broadcast address
`ff:ff:ff:ff:ff:ff` — the switch sends the frame out every port except
the one it came in on. This is the fallback, not the default.

### Dropping

If a frame arrives destined for a MAC that lives on the same port it came
in on, the switch drops it. The destination is already on that segment —
forwarding would be pointless.

### Aging

MAC addresses don't live forever. Machines get unplugged, moved between
ports, or replaced. If the switch kept stale entries forever, it would
forward frames to the wrong port indefinitely.

Every entry has a timestamp. Entries older than `max_age_seconds` are
evicted. The standard value in real switches is 300 seconds.

When an entry is evicted, we swap it with the last entry and decrement
count — O(1) deletion with no shifting. Order in the table doesn't matter
because lookup is always a linear scan anyway.

## The Three Outcomes

Given any incoming frame, a switch does exactly one of these:

| Outcome | Condition | Action |
|---------|-----------|--------|
| FLOOD | Destination unknown or broadcast | Send out all ports except incoming |
| FORWARD | Destination known, different port | Send out that port only |
| DROP | Destination known, same port | Discard |

## Data Structures

### `mac_table_entry`

One row in the forwarding table:

```c
typedef struct mac_table_entry {
    uint8_t  mac[6];       // the MAC address
    uint8_t  port;         // which port it lives on (max 48 ports → uint8_t)
    time_t   last_seen;    // for aging
} mac_table_entry;
```

### `forwarding_table`

The full table:

```c
typedef struct forwarding_table {
    mac_table_entry table[1024];  // fixed-size array
    uint16_t        count;        // current number of entries
} forwarding_table;
```

Maximum 1024 entries — a deliberate cap. When the table is full and a new
MAC arrives, we evict the oldest entry to make room. This is a simplified
eviction policy; real switches use more sophisticated CAM hardware.

## Design Decisions

**Linear scan for lookup.** With 1024 entries worst case this is fine.
A production switch uses a CAM (Content Addressable Memory) — hardware
that searches all entries in parallel in a single clock cycle. We don't
have CAM, so we scan. Known limitation.

**Swap-with-last for deletion.** When aging removes an entry, we fill
its slot with the last entry and decrement count. O(1) vs O(n) for
shifting. Order doesn't matter because lookup is linear anyway.

**`find_oldest` is `static`.** It's an internal helper, not part of the
public interface. `static` makes it invisible outside `switch.c`. Anything
the outside world doesn't need to call should be static.

**`payload_len` lives outside the struct.** `ethernet_frame` is a wire
format — every byte has a position real NICs and switches agree on.
Adding bookkeeping fields would break every cast against real packets.

## What We Tested
Frame 1: broadcast destination (ff:ff:ff:ff:ff:ff), arriving on port 1
→ [FLOOD] to ports 2 and 3. Switch learns sender on port 1.
Frame 2: destination is a known MAC (learned from frame 1), arriving on port 1
→ [DROP]. Destination lives on port 1, same as incoming.
Frame 3: same destination, now arriving on port 3
→ [FORWARD] to port 1. Destination is known and on a different port.

All three outcomes verified in a single test run.

## Known Limitations / TODOs

- [ ] No packet captures yet — run with tcpdump in Chapter 3 when real
      interfaces are involved
- [ ] No Wireshark screenshots — planned from Chapter 3 onward
- [ ] Linear scan O(n) lookup — production uses CAM hardware
- [ ] Single eviction policy (oldest) — LRU or LFU would be more realistic
- [ ] No VLAN support — real switches isolate broadcast domains per VLAN
- [ ] No STP (Spanning Tree Protocol) — loops in the topology would cause
      broadcast storms

## RFC / Standards References

- IEEE 802.1D — MAC Bridges (defines the learning/forwarding/flooding model)
- IEEE 802.1Q — VLAN tagging (future chapter)

## Files

- `switch.h` — struct definitions, constants, public prototypes
- `switch.c` — table_init, table_learn, table_lookup, table_age,
               switch_process_frame
- `main.c`   — tests for all three forwarding outcomes
- `ethernet.h` — borrowed from Chapter 1 (shared include path planned)