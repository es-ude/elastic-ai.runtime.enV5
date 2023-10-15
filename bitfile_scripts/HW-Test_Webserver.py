from io import BytesIO
from flask import Flask, send_file

bytes_per_request = 512  # should be matching with the page size of the flash

app = Flask(__name__)


def print_bytearray(arr: bytes):
    print("0x" + " 0x".join("%02X" % b for b in arr))


def read_slice(position: int, filename: str) -> bytes:
    with open(filename, "rb") as file:
        file.seek(position * bytes_per_request, 0)
        chunk: bytes = file.read(bytes_per_request)
        print(f"Chunk {position}:")
        print_bytearray(chunk)
    return chunk


@app.route("/getfast/<position>")
def get_file_fast(position: str):
    """
    Using positional arguments as parameter did not work!
    """
    buffer = BytesIO()
    buffer.write(
        read_slice(int(position), "bitfiles/env5_bitfiles/blink_fast/led_test.bin")
    )
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name="bitfile.bin",
        mimetype="application/octet-stream",
    )


@app.route("/getslow/<position>")
def get_file_slow(position: str):
    """
    Using positional arguments as parameter did not work!
    """
    buffer = BytesIO()
    buffer.write(
        read_slice(int(position), "bitfiles/env5_bitfiles/blink_slow/led_test.bin")
    )
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name="bitfile.bin",
        mimetype="application/octet-stream",
    )


@app.route("/check")
def check_server_available():
    return "AVAILABLE\0"


if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
