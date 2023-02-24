from flask import Flask, send_file, request
from io import BytesIO
import json

app = Flask(__name__)


def read_slice(position: int, filename: str) -> bytes:
    with open(filename, "rb") as file:
        file.seek(position * 256)
        chunk: bytes = file.read(256)
    return chunk


@app.route('/getfile/<position>')
def get_file(position: str):
    """
    Using positional arguments as parameter did not work!
    """
    buffer = BytesIO()
    buffer.write(read_slice(int(position), "file.txt"))
    buffer.seek(0)
    return send_file(
        buffer,
        as_attachment=True,
        download_name='bitfile.bit',
        mimetype='application/octet-stream')


@app.route('/check')
def check_server_available():
    return "AVAILABLE"


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)