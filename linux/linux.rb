require 'RMagick'
require 'socket'
require 'base64'

tcp_server = TCPSocket.new '192.241.213.182', '61400'

loop do
    image = Magick::Image.capture {self.filename = 'root'}
    image.write('image.jpg')
    image = Magick::Image.read('image.jpg').first
    image = image.adaptive_resize(0.6)
    blob = image.to_blob {self.quality = 50}
    base64_image = Base64.encode64(blob)

    base64_image.gsub!(/\n/, '')

    print base64_image.size

    tcp_server.puts "image=#{base64_image}&id=linux&time=#{Time.now.to_i}&v=1&\n"

    puts ' OK'

    GC.start
end
