import http.server
import socketserver
import sys
import json

class MockPetstoreHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json')
        self.end_headers()
        if self.path.endswith('swagger.json'):
            self.wfile.write(b'{"swagger": "2.0"}')
        else:
            self.wfile.write(b'{}')

    def do_POST(self):
        self.do_GET()
    def do_PUT(self):
        self.do_GET()
    def do_DELETE(self):
        self.do_GET()
    def do_PATCH(self):
        self.do_GET()

if __name__ == '__main__':
    port = int(sys.argv[1])
    with socketserver.TCPServer(("", port), MockPetstoreHandler) as httpd:
        httpd.serve_forever()
