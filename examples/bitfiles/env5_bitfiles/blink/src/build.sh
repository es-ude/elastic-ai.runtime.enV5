#!/usr/bin/env bash
# Builds the 1 Hz and 2 Hz LED blink bitfiles for the Spartan-7 XC7S50 of the elastic-node.
#
# Uses the container from python_utils/bitfile_scripts/bitfiles/openxc7 (requires Docker)
# and copies the results to ../xc7s50.
#
# Usage: ./build.sh [blink_hz ...]    (default: 1 2)
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bitfiles_dir="$(cd "${script_dir}/../../.." && pwd)"
out_dir="$(cd "${script_dir}/.." && pwd)/xc7s50"
image=enuo-openxc7
frequencies=("$@")
[[ ${#frequencies[@]} -gt 0 ]] || frequencies=(1 2)

docker build -t "$image" "${bitfiles_dir}/openxc7"
mkdir -p "$out_dir"

for hz in "${frequencies[@]}"; do
  docker run --rm -v "${bitfiles_dir}":/work -w /work "$image" bash openxc7/build.sh \
    top=led_blink name="led_blink_${hz}hz" \
    src=env5_bitfiles/blink/src/led_blink.v \
    xdc=env5_bitfiles/blink/src/constraints.xdc \
    param.CLK_HZ=100000000 param.BLINK_HZ="${hz}" freq_mhz=100 \
    out=openxc7/build
  cp "${bitfiles_dir}/openxc7/build/led_blink_${hz}hz".{bit,bin} "$out_dir/"
done

echo "Bitfiles written to ${out_dir}"
