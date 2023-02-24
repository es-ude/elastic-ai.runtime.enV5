from flask import Flask, send_file, request
from io import BytesIO
import json

app = Flask(__name__)


def read_slice(position):
    filename = 'file.txt'
    openfile = open(filename, 'rb')
    openfile.seek(position * 256)
    chunk = openfile.read(100)
    openfile.close()
    return chunk


# @app.route('/getfile')
# def get_file():
# chunk = read_slice()
# return send_file(chunk, as_attachment=True, attachment_filename="bitfile.bit")

##
# Using positional arguments as parameter did not work
##

@app.route('/getfile/<pos>')
def get_file(pos):
    # position = request.args.get('pos', default = None, type = int)
    buffer = BytesIO()
    buffer.write(read_slice(int(pos)))
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
