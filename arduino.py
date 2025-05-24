import network
import socket
import machine

# change pints
red = machine.PWM(machine.Pin(14), freq=1000)
green = machine.PWM(machine.Pin(12), freq=1000)
blue = machine.PWM(machine.Pin(13), freq=1000)

def set_color(r, g, b):
    red.duty(int(r * 4))
    green.duty(int(g * 4))
    blue.duty(int(b * 4))

ssid = 'WIFI_SSID'
password = 'PASSWORD'

station = network.WLAN(network.STA_IF)
station.active(True)
station.connect(ssid, password)

while not station.isconnected():
    pass

print('Connected. IP:', station.ifconfig()[0])

# Basic web server
addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
s = socket.socket()
s.bind(addr)
s.listen(1)

print('Listening on', addr)

def parse_query(query):
    try:
        params = dict(qc.split('=') for qc in query.split('&'))
        r = int(params.get('r', 0))
        g = int(params.get('g', 0))
        b = int(params.get('b', 0))
        return r, g, b
    except Exception as e:
        print("Error parsing query:", e)
        return None

while True:
    cl, addr = s.accept()
    print('Client connected from', addr)
    request = cl.recv(1024).decode()
    print("Request:", request)

    if 'GET /setColor?' in request:
        try:
            query_string = request.split('GET /setColor?')[1].split(' ')[0]
            color = parse_query(query_string)
            if color:
                set_color(*color)
                response = 'HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nColor set'
            else:
                response = 'HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain\r\n\r\nInvalid parameters'
        except Exception as e:
            response = 'HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\n\r\nError'
    else:
        response = 'HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found'

    cl.send(response)
    cl.close()
