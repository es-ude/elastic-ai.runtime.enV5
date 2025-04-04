import shutil as sh
import argparse
import time

if __name__ == "__main__":
    """example call 
        python -m elasticai.testing.hw_test -d /media/jan/RPI-RP2/ -p /dev/ttyACM0 -t ../build/env5_rev2_debug/test/hardware/Esp/HWTestAskEspForOkViaPico.uf2
        
        Help:
        execute `python3 ./hw_tst.py --help`
    """

    parser = argparse.ArgumentParser(prog="HW-Test", description="Hardware-Test Runner")
    parser.add_argument("-p", "--port", required=True, help="Device COM-Port")
    parser.add_argument("-t", "--test", required=True, help="Path to hardware-test executables")
    parser.add_argument("-d", "--dir", required=True, help=" Path to directory where device is located")
    args = parser.parse_args()
    sh.copy(args.test, args.dir)
    time.sleep(2)
    with open(args.port, "r") as serial:
        while serial.isatty():
            line = serial.readline()
            if line != "":
                print(line.strip().strip("\n"), end=None)