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
      pkgs.nanomq
      pkgs.vale
    ];

  languages.c.enable = true;
  languages.python = {
    enable = true;
    package = pkgs.python312;
    venv.enable = true;
    venv.quiet = true;
    uv = {
      enable = true;
      package = unstablePkgs.uv;
      sync.enable = true;
      sync.allExtras = true;
    };
  };

  scripts = {
    start_mqtt_broker = {
      exec = "nanomq start -d -p 1883";
      package = pkgs.bash;
      description = "start nanomq broker";
    };
    stop_mqtt_broker = {
      exec = "nanomq stop";
      package = pkgs.bash;
      description = "stop nanomq broker";
    };
    publish = {
      exec = ''nanomq_cli pub -t "$1" -m "$2"'';
      package = pkgs.bash;
      description = "publish message";
    };
    publish_retain = {
      exec = ''nanomq_cli pub -t "$1" -m "$2" -r'';
      package = pkgs.bash;
      description = "publish retained message";
    };
    subscribe = {
      exec = ''nanomq_cli sub -t "$1"'';
      package = pkgs.bash;
      description = "subscribe to topic";
    };
    setup_cmake = {
      exec = ''
        cmake --preset unit_test
        cmake --preset env5_rev2_debug
        cmake --preset env5_rev2_release
      '';
      package = pkgs.bash;
      description = "setup cmake";
    };
    clean_cmake = {
      exec = ''
        cmake --build --target clean --preset unit_test
        cmake --build --target clean --preset env5_rev2_debug
        cmake --build --target clean --preset env5_rev2_release
      '';
      package = pkgs.bash;
      description = "clean cmake";
    };
    build_unit_tests = {
      exec = ''
        if [ -z "$1" ]; then
          cmake --build --preset unit_test --target "$1"
        else
          cmake --build --preset unit_test
        fi
      '';
      package = pkgs.bash;
      description = "build unit-tests";
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
              ( exec picotool load -v -x -f "$1" >/dev/null 2>&1 )
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
    run_test = {
      exec = ''
        flash_node "build/env5_rev2_release/test/hardware/AutomatedHWTests/$1.uf2"
        echo "Running Test: $1"
        sleep 2
        while read -r line; do
          if [[ -n "$2" ]]; then
            echo "$line"
          fi
          if [[ -n "$line" ]]; then
            if [[ "$line" == "OK" ]]; then
              echo "OK" > /tmp/test_result_$1
              echo -e "  \e[32mOK\e[0m"
              break
            elif [[ "$line" == "FAIL" ]]; then
              echo "FAIL" > /tmp/test_result_$1
              echo -e "  \e[31mFAIL\e[0m"
              break
            fi
          fi
        done < /dev/ttyACM0
      '';
      package = pkgs.bash;
      description = "run one test (with or without debug output) and print the result";
    };
    run_tests = {
      exec = ''
        TESTS="TestFreeRTOSTask TestFreeRTOSQueue TestFreeRTOSTaskDualCore TestFilesystem TestFlash TestFPGACommunication"
        test_results=""

        for test in $TESTS; do
          echo "Running test" $test
          run_test "$test" > /dev/null
          result_file="/tmp/test_result_$test"
          if [[ -f "$result_file" ]]; then
            result=$(cat "$result_file")

            if [[ "$result" == "OK" ]]; then
              test_results+="  \e[32mOK\e[0m    | $test"$'\n'
            elif [[ "$result" == "FAIL" ]]; then
              test_results+="  \e[31mFAIL\e[0m  | $test"$'\n'
            else
              test_results+="$result | $test"$'\n'
            fi

            rm "$result_file"
          else
            test_results+="  ERROR | $test"$'\n'
          fi
        done

        echo "----------------"
        echo -e "Result:\n$test_results"
      '';
      package = pkgs.bash;
      description = "run all hardware tests and print their result";
    };
  };

  tasks = {
    "prepare:cmake" = {
      exec = ''
        setup_cmake
        clean_cmake
      '';
      before = [ "build:unit-test" "build:debug-targets" "build:release-targets" ];
    };
    "build:unit-test" = {
        exec = ''
          build_unit_tests
        '';
      before = [ "check:unit-test" ];
    };
    "build:debug-targets" = {
        exec = ''
          build_pico_debug
        '';
      before = [ "check:build-pico-targets" ];
    };
    "build:release-targets" = {
        exec = ''
          build_pico_release
        '';
      before = [ "check:build-pico-targets" ];
    };
    "check:unit-test" = {
      exec = ''
        ctest --preset unit_test
      '';
    };
    "check:build-pico-targets" = {
      exec = '''';
    };
    "check:commit-lint" = {
      exec = ''
        ${pkgs.cocogitto}/bin/cog check --from-latest-tag --ignore-merge-commits
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
  };

  enterShell = ''
    echo
    echo "Welcome back"
    echo
  '';
}
