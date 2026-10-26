#!/usr/bin/env bash
# Builds a bitfile from Verilog sources for the elastic-node FPGA with the open source toolchain
# (yosys, nextpnr-xilinx, Project X-Ray). Runs inside the container, see ../README.md.
#
# Usage: build.sh top=<module> src=<file>[,<file>...] xdc=<file> [key=value ...]
#
# Required:
#   top        top module
#   src        Verilog or SystemVerilog source files, separated by commas
#   xdc        constraints file (pins and I/O standards)
# Optional:
#   name       base name of the output files               (default: <top>)
#   part       FPGA part                                   (default: xc7s50ftgb196-1)
#   freq_mhz   target clock frequency in whole MHz         (default: 100)
#   out        output directory                            (default: build)
#   param.<N>  override parameter <N> of the top module, can be given several times
#
# Relative paths are resolved from the current directory.
# Output: <out>/<name>.bit and <name>.bin (and .json, .fasm, .frames)
set -euo pipefail

top=""
sources=""
xdc=""
name=""
part=xc7s50ftgb196-1
freq_mhz=100
out=build
chparams=""

for arg in "$@"; do
    case "$arg" in
        top=*) top="${arg#*=}" ;;
        src=*) sources="${arg#*=}" ;;
        xdc=*) xdc="${arg#*=}" ;;
        name=*) name="${arg#*=}" ;;
        part=*) part="${arg#*=}" ;;
        freq_mhz=*) freq_mhz="${arg#*=}" ;;
        out=*) out="${arg#*=}" ;;
        param.*=*) key="${arg#param.}"; chparams+=" -set ${key%%=*} ${key#*=}" ;;
        *) echo "invalid argument '$arg'" >&2; exit 1 ;;
    esac
done

[[ -n "$top" && -n "$sources" && -n "$xdc" ]] || { echo "top, src and xdc are required" >&2; exit 1; }
[[ -f "$xdc" ]] || { echo "constraints file '$xdc' not found" >&2; exit 1; }
name="${name:-$top}"

read_commands=""
IFS=',' read -ra source_files <<< "$sources"
for file in "${source_files[@]}"; do
    [[ -f "$file" ]] || { echo "source file '$file' not found" >&2; exit 1; }
    read_commands+="read_verilog -sv ${file}"$'\n'
done
chparam_command=""
[[ -z "$chparams" ]] || chparam_command="chparam${chparams} ${top}"

device="${part%-*}"          # e.g. xc7s50ftgb196
mkdir -p "$out"

db_root="$(find /nix/store -maxdepth 2 -type d -name spartan7 -path '*-source/*' | head -n1)"
chipdb="$(find /nix/store -name "${device}.bin" -path '*chipdb*' | head -n1)"
[[ -d "${db_root}/${part}" ]] || { echo "part ${part} not in the Project X-Ray database" >&2; exit 1; }
[[ -n "$chipdb" ]] || { echo "no chipdb for ${device}; the Project X-Ray database only has fabric data for XC7S25, XC7S50 and XC7S100" >&2; exit 1; }

# fasm2frames cannot see the prjxray, fasm and dependency modules on its own.
prjxray_python="$(find /nix/store -maxdepth 4 -type d -path '*-prjxray-*/usr/share/python3' | head -n1)"
fasm_python="$(find /nix/store -maxdepth 5 -type d -path '*-python3.12-fasm/lib/python3.12/site-packages' | head -n1)"
env_python="$(find /python-env-path/ -maxdepth 4 -type d -name site-packages | head -n1)"
PYTHONPATH="${prjxray_python}:${fasm_python}:${env_python}"
export PYTHONPATH

yosys -q -p "
    ${read_commands}
    ${chparam_command}
    synth_xilinx -flatten -abc9 -arch xc7 -top ${top}
    write_json ${out}/${name}.json
"

nextpnr-xilinx --chipdb "$chipdb" --xdc "$xdc" \
    --json "${out}/${name}.json" --fasm "${out}/${name}.fasm" \
    --freq "$freq_mhz"

fasm2frames --part "$part" --db-root "$db_root" "${out}/${name}.fasm" > "${out}/${name}.frames"
xc7frames2bit --part_file "${db_root}/${part}/part.yaml" --part_name "$part" \
    --frm_file "${out}/${name}.frames" --output_file "${out}/${name}.bit"

# The .bin is the raw configuration data: the .bit without its header.
"${env_python%/lib/*}/bin/python3" - "${out}/${name}.bit" "${out}/${name}.bin" <<'PY'
import struct
import sys

data = open(sys.argv[1], "rb").read()
pos = 2 + struct.unpack(">H", data[:2])[0] + 2  # magic block and the 0x0001 marker
for key in b"abcd":  # design name, part, date and time, each with a 16 bit length
    assert data[pos] == key, f"unexpected field {chr(data[pos])}"
    pos += 3 + struct.unpack(">H", data[pos + 1:pos + 3])[0]
assert data[pos] == ord("e"), "missing bitstream field"
size = struct.unpack(">I", data[pos + 1:pos + 5])[0]
open(sys.argv[2], "wb").write(data[pos + 5:pos + 5 + size])
PY

echo "Done: ${out}/${name}.bit and .bin"
