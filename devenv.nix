{ pkgs, lib, config, inputs, ... }:

let
  unstablePkgs = import inputs.nixpkgs-unstable { system = pkgs.stdenv.system; };
in
{

  packages = let
    u = unstablePkgs;
   in
    [
        pkgs.cmake
        pkgs.ninja
        pkgs.clang-tools
        u.gcc-arm-embedded-13
        u.ruff
        u.mypy
        u.picotool
    ];


  languages.c.enable = true;
  languages.python = {
    enable = true;
    package = unstablePkgs.python312;
    uv.enable = true;
    uv.package = unstablePkgs.uv;
  };

  scripts.hello.exec = ''
    echo hello from $GREET
  '';

  scripts.run_tests.exec = ''
    cmake --build --preset unit_test
    ctest --preset unit_test
  '';

  enterShell = ''
    hello
    cmake --version
  '';

}
