require 'socket'

socket = UDPSocket.new
socket.bind('192.168.0.118', 61000)

loop do
    # text, sender = socket.recvfrom(110000)
    text, sender = socket.recvfrom(32)
    puts text
    puts text.size
end
