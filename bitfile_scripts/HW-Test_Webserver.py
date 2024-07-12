from pathlib import Path
from io import BytesIO
from flask import Flask, send_file, request

app: Flask = Flask(__name__)
bin_file_source_dir: Path = Path(__file__).resolve().parent


def print_bytearray(arr: bytes):
    print("0x" + " 0x".join("%02X" % b for b in arr))


def read_slice(position: int, filename: Path, max_length: int = 512) -> bytes:
    with open(filename, "rb") as file:
        file.seek(position * max_length, 0)
        chunk: bytes = file.read(max_length)
    return chunk


@app.route("/getfast")
def get_file_fast():
    chunk_size = request.args.get("chunkMaxSize", type=int)
    chunk_number = request.args.get("chunkNumber", type=int)

    print(f"chunk_size: {chunk_size}, chunk_number: {chunk_number}")

    buffer = BytesIO()
    buffer.write(
        read_slice(
            chunk_number,
            bin_file_source_dir.joinpath(
                "bitfiles/env5_bitfiles/blink/blink_fast/led_test.bin"
            ),
            chunk_size,
        )
    )
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name="bitfile.bin",
        mimetype="application/octet-stream",
    )


@app.route("/getslow")
def get_file_slow():
    chunk_size = request.args.get("chunkMaxSize", type=int)
    chunk_number = request.args.get("chunkNumber", type=int)

    buffer = BytesIO()
    buffer.write(
        read_slice(
            chunk_number,
            bin_file_source_dir.joinpath(
                "bitfiles/env5_bitfiles/blink/blink_slow/led_test.bin"
            ),
            chunk_size,
        )
    )
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name="bitfile.bin",
        mimetype="application/octet-stream",
    )


@app.route("/getecho")
def get_file_echo():
    chunk_size = request.args.get("chunkMaxSize", type=int)
    chunk_number = request.args.get("chunkNumber", type=int)

    buffer = BytesIO()
    buffer.write(
        read_slice(
            chunk_number,
            bin_file_source_dir.joinpath(
                "bitfiles/env5_bitfiles/echo_server/env5_top_reconfig.bin"
            ),
            chunk_size,
        )
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
