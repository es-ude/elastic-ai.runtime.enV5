{ pkgs, lib, config, inputs, ... }:

let
  unstablePkgs = import inputs.nixpkgs-unstable { system = pkgs.stdenv.system; };
in
{
  packages = let
    u = unstablePkgs;
  in
    [
      pkgs.git
      pkgs.cocogitto
      pkgs.clang-tools
      pkgs.gcc
      pkgs.cmake
      pkgs.ninja
      pkgs.graphviz
      u.gcc-arm-embedded-13
      u.picotool
      pkgs.minicom
    ];

  languages.c.enable = true;
  languages.python = {
    enable = true;
    package = unstablePkgs.python312;
    uv.enable = true;
    uv.package = unstablePkgs.uv;
  };

  scripts = {
    setup_cmake = {
      exec = ''
        cmake --preset unit_test
        cmake --preset env5_rev2_debug
        cmake --preset env5_rev2_release
      '';
      package = pkgs.bash;
      description = "setup cmake";
    };
    build_pico_release = {
      exec = ''
        if [ -z "$1" ]; then
          cmake --build --preset env5_rev2_release --target "$1"
        else
          cmake --build --preset env5_rev2_release
        fi
      '';
      package = pkgs.bash;
      description = "build pico target of type RELEASE";
    };
    build_pico_debug = {
      exec = ''
        if [ -z "$1" ]; then
          cmake --build --preset env5_rev2_debug --target "$1"
        else
          cmake --build --preset env5_rev2_debug
        fi
      '';
      package = pkgs.bash;
      description = "build pico target of type DEBUG";
    };
    flash_node = {
      exec = ''
        if [ -e "$1" ]; then
          if [ -r "$1" ]; then
            if [[ "$1" == *.uf2 ]]; then
              picotool load -f "$1"
            else
              echo "Not a valid file type (UF2)!"
              exit 1
            fi
          else
            echo "Can't read file!"
            exit 1
          fi
        else
          echo "You must provide an existing file to load!"
          exit 1
        fi
      '';
      package = pkgs.bash;
      description = "flash given UF2 file to pico";
    };
    format_files = {
      exec = ''
        INCLUDE_REGEX="^.*\.((((c|C)(c|pp|xx|\+\+)?$)|((h|H)h?(pp|xx|\+\+)?$))|(ino|pde|proto|cu))$"
        SRC_DIRECTORIES=("./src" "./test")
        SRC_FILES=$(find $SRC_DIRECTORIES -name .git -prune -o -regextype posix-egrep -regex "$INCLUDE_REGEX" -print)
        for file in $SRC_FILES; do
          clang-format -i -Werror --style=file --fallback-style="llvm" $file
        done
      '';
      package = pkgs.bash;
      description = "apply clang-format to src,test directory";
    };
  };

  tasks = {
    "build:unit-test" = {
      exec = ''
        cmake --preset unit_test
        cmake --build --clean-first --preset unit_test
      '';
    };
    "build:pico-release-targets" = {
      exec = ''
        cmake --preset env5_rev2_release
        cmake --build --clean-first --preset env5_rev2_release
      '';
    };
    "build:pico-debug-targets" = {
      exec = ''
        cmake --preset env5_rev2_debug
        cmake --build --clean-first --preset env5_rev2_debug
      '';
    };
    "check:commit-lint" = {
      exec = ''
        if [ -n "$CI" ]; then
          ${pkgs.cocogitto}/bin/cog check ..$GITHUB_SOURCE_REF
        else
          ${pkgs.cocogitto}/bin/cog check --from-latest-tag --ignore-merge-commits
        fi
      '';
    };
    "check:code-lint" = {
      exec = ''
        INCLUDE_REGEX="^.*\.((((c|C)(c|pp|xx|\+\+)?$)|((h|H)h?(pp|xx|\+\+)?$))|(ino|pde|proto|cu))$"
        SRC_DIRECTORIES=("./src" "./test")
        SRC_FILES=$(find $SRC_DIRECTORIES -name .git -prune -o -regextype posix-egrep -regex "$INCLUDE_REGEX" -print)
        echo FILES="$SRC_FILES"
        for file in $SRC_FILES; do
          clang-format --dry-run -Werror --style=file --fallback-style="llvm" $file
        done
      '';
    };
    "check:unit-test" = {
      exec = ''
        ctest --preset unit_test
      '';
      after = ["build:unit-test"];
    };
    "check:build-pico-targets" = {
      after = ["build:pico-release-targets" "build:pico-debug-targets"];
    };
  };

  enterShell = ''
    cmake --version
    gcc --version
    arm-none-eabi-gcc --version
  '';

}
