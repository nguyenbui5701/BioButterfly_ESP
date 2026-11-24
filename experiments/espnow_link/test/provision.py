"""Generate PRIVATE ignored local configuration; no secrets printed."""
from pathlib import Path
import re
import secrets

def mac(role):
    s = input(role + " STA MAC (from own board identification): ").strip()
    if not re.fullmatch(r"(?:[0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2}", s):
        raise SystemExit("Invalid MAC format")
    value = bytes.fromhex(s.replace(":", ""))
    if value[0] & 1 or not any(value):
        raise SystemExit("Require nonzero unicast MAC")
    return value

def array(name, value):
    return "static const uint8_t " + name + "[] = {" + ",".join("0x%02x" % n for n in value) + "};\n"

path = Path(__file__).resolve().parents[1] / "include" / "local_config.h"
if path.exists():
    raise SystemExit("Local config already exists; keep it for both builds. Move it privately before regenerating.")
tx, rx = mac("TX"), mac("RX")
if tx == rx:
    raise SystemExit("TX and RX must differ")
with path.open("x", encoding="ascii") as f:
    f.write("#pragma once\n#include <stdint.h>\n")
    f.write(array("tx_mac", tx) + array("rx_mac", rx))
    f.write(array("pmk", secrets.token_bytes(16)) + array("lmk", secrets.token_bytes(16)))
print("Local config created. Do not share or commit it.")
