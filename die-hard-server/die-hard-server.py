#!/usr/local/bin/python

import SocketServer
import BaseHTTPServer
import threading
import time
import signal
import ssl
import sys


# A Python server for simulating different health scenarios.
# Useful for testing health checking.


# Optional parameters store.
optional = dict()


# A response helper.
def construct_http_response(s, status):
    s.send_response(status)
    s.send_header("Content-Length", "0")
    s.send_header("Content-Type", "text/html; encoding=utf8")
    s.end_headers()


def healthy(s):
    construct_http_response(s, 204)

def unhealthy(s):
    construct_http_response(s, 503)

def emulate(s, n):
    # Start "unhealthy task emulation" by storing the number of
    # first consecutive success responses.
    if not "success-count" in optional:
        print "Start unhealthy task emulation"
        optional["success-count"] = n

    # Determine how many time we should respond with success, if any.
    # Based on how many times we have already responded with success,
    # respond with either success or failure.
    if optional["success-count"] > 0:
        print "{} healthy responses to send".format(optional["success-count"])
        optional["success-count"] = optional["success-count"] - 1
        construct_http_response(s, 200)
    else:
        construct_http_response(s, 500)

def not_supported(s):
    print "POST requests are not supported"
    construct_http_response(s, 404)

# GET to / returns 204.
# GET to /healthy returns 204.
# GET to /unhealthy returns 503.
# GET to /emulate/<n> returns 200 first <n> times and 500 afterwards;
# this emulates a task transitioning from healthy to unhealthy.
# POST requests are explicitly not supported, 404 is returned.
# All other request methods are implicitly not supported, 501
# is returned.
class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    def do_GET(s):
        # E.g. /emulate/3
        tokens = s.path.split("/")

        if tokens[1] == "":
            healthy(s)
        elif tokens[1] == "healthy":
            healthy(s)
        elif tokens[1] == "unhealthy":
            unhealthy(s)
        elif tokens[1] == "emulate":
            emulate(s, int(tokens[2]))

    def do_POST(s):
        not_supported(s)


class TCPHandler(SocketServer.BaseRequestHandler):
    def handle(self):
        # self.request is the TCP socket connected to the client
        print "Request from {}".format(self.client_address[0])


        # Prepare a "200 OK" response.
        response_code = "200 OK"

        response_headers = {
            "Content-Type": "text/html; encoding=utf8",
            "Content-Length": "0",
            "Date": time.strftime("%a, %d %b %Y %H:%M:%S GMT", time.gmtime()),
            "Server": "Python Health Check Task"
        }

        response_headers_encoded = "".join("{}: {}\r\n".format(k, v) \
            for k, v in response_headers.iteritems())

        # sending all this stuff
        response = "HTTP/1.1 {}\r\n".format(response_code) + response_headers_encoded;
        self.request.sendall(response)


def shutdown(signal, frame):
    print "Goodbye!"
    sys.exit(0)


if __name__ == "__main__":
    HOST, TCP_PORT, HTTP_PORT = "localhost", 8000, 8888

    # Create servers.
    tcp_server = SocketServer.TCPServer((HOST, TCP_PORT), TCPHandler)
    http_server = BaseHTTPServer.HTTPServer((HOST, HTTP_PORT), HTTPHandler)
    #http_server.socket = ssl.wrap_socket(http_server.socket)

    # Activate the server; this will keep running until you
    # interrupt the program with Ctrl-C
    print "Serving TCP on {}:{}".format(HOST, TCP_PORT)
    tcp_thread = threading.Thread(target = tcp_server.serve_forever)
    tcp_thread.daemon = True
    tcp_thread.start()
    #tcp_server.serve_forever()

    print "Serving HTTP on {}:{}".format(HOST, HTTP_PORT)
    http_thread = threading.Thread(target = http_server.serve_forever)
    http_thread.daemon = True
    http_thread.start()
    #http_server.serve_forever()

    print "(Listening for Ctrl-C)"
    signal.signal(signal.SIGINT, shutdown)
    while True:
        time.sleep(1)
