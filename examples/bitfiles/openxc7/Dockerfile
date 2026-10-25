# Open-source FPGA toolchain (yosys, nextpnr-xilinx, prjxray) in a Linux container.
# Used because the openXC7 packages do not build on macOS.
FROM nixos/nix:latest

RUN echo "experimental-features = nix-command flakes" >> /etc/nix/nix.conf \
 && echo "extra-substituters = https://openxc7.cachix.org" >> /etc/nix/nix.conf

# Pinned revision of github:openXC7/toolchain-nix, update deliberately.
ARG OPENXC7_REV=42323cb00c8b76486619695166498dfa72b1e3e2

RUN nix profile install \
      "github:openXC7/toolchain-nix/${OPENXC7_REV}#nextpnr" \
      "github:openXC7/toolchain-nix/${OPENXC7_REV}#nextpnr-xilinx-chipdb.spartan7" \
      "github:openXC7/toolchain-nix/${OPENXC7_REV}#prjxray" \
      "github:openXC7/toolchain-nix/${OPENXC7_REV}#prjxray-db" \
      "github:openXC7/toolchain-nix/${OPENXC7_REV}#fasm" \
      nixpkgs#yosys nixpkgs#openfpgaloader nixpkgs#gnused

# Python modules that fasm2frames and xc7frames2bit need (prjxray, fasm and their dependencies).
RUN nix build --no-link --print-out-paths --impure --expr \
      'let pkgs = import <nixpkgs> {}; in pkgs.python312.withPackages (ps: [ ps.pyyaml ps.textx ps.intervaltree ps.simplejson ps.sortedcontainers ps.pyjson5 ps.cython ps.setuptools ])' \
      > /python-env \
 && ln -s "$(cat /python-env)" /python-env-path

WORKDIR /work
