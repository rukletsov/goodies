import time
import http.server as server

HOST_NAME = "127.0.0.1"
PORT_NUMBER = 9000


class MyHandler(server.BaseHTTPRequestHandler):
    def do_GET(s):
        """Respond to a GET request."""
        print("Received request from {}".format(s.client_address))
        s.send_response(200)
        time.sleep(60*60)
        s.send_header("Content-type", "text/html")
        s.end_headers()
        s.wfile.write("here is your joke")

if __name__ == '__main__':
    server_class = server.HTTPServer
    httpd = server_class((HOST_NAME, PORT_NUMBER), MyHandler)
    try:
        print("Server started on {}:{}".format(HOST_NAME, PORT_NUMBER))
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()