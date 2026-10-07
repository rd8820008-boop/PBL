import socket
s = socket.socket(socket.AF_UNIX)
s.connect('/tmp/sim_logger.sock')
s.send(b'hello from test\n')
s.close()
print("sent")
