import json, subprocess, os
from http.server import BaseHTTPRequestHandler, HTTPServer

HERE = os.path.dirname(os.path.abspath(__file__))

def flat(b):
    t = [b['cmd'], b['n'], b['m']] + b['available']
    for k in ('alloc', 'max', 'request'):
        for row in b[k]:
            t += row
    if b['cmd'] == 'request':
        t += [b['p']] + b['req']
    return ' '.join(map(str, t))

class H(BaseHTTPRequestHandler):
    def do_GET(self):
        data = open(os.path.join(HERE, 'index.html'), 'rb').read()
        self.send_response(200); self.send_header('Content-Type', 'text/html'); self.end_headers()
        self.wfile.write(data)
    def do_POST(self):
        body = json.loads(self.rfile.read(int(self.headers['Content-Length'])))
        r = subprocess.run([os.path.join(HERE, 'deadlock')], input=flat(body), capture_output=True, text=True)
        self.send_response(200); self.send_header('Content-Type', 'application/json'); self.end_headers()
        self.wfile.write((r.stdout or '{"error":"C program failed"}').encode())
    def log_message(self, *a): pass

print('Open the forwarded port 5000 in your browser')
HTTPServer(('0.0.0.0', 5000), H).serve_forever()