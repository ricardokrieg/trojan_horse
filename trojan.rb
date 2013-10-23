require 'RMagick'
require 'base64'
require 'net/http'
require 'rbconfig'

include Magick

def os
    @os ||= (
        host_os = RbConfig::CONFIG['host_os']

        case host_os
        when /mswin|msys|mingw|cygwin|bccwin|wince|emc/
            :windows
        when /darwin|mac os/
            :macosx
        when /linux/
            :linux
        when /solaris|bsd/
            :unix
        else
            raise Error, "unknown os: #{host_os.inspect}"
        end
    )
end

def capture
    if os == :windows
        require 'win32ole'
        require 'Win32API'

        keybd_event = Win32API.new('user32', 'keybd_event', ['I','I','L','L'], 'V')

        active_window_only = false
        if not active_window_only
            keybd_event.Call(0x2C,0,0,0) # Print Screen
        else
            keybd_event.Call(0x2C,1,0,0) # Alt+Print Screen
        end

        `convert clipboard: image.jpg`

        Image.read('image.jpg').first
    else
        image = Image.capture {self.filename = 'root'}
        image.format = 'jpg'

        image
    end
end

500.times do
    # image = Image.capture {self.filename = 'root'}
    # image.format = 'jpg'
    image = capture

    image.scale!(0.8)
    image64 = Base64.encode64(image.to_blob)
    image.destroy!

    # Net::HTTP.post_form(URI.parse('http://192.241.213.182:4567'), {image: image64})
    # Net::HTTP.post_form(URI.parse('http://127.0.0.1:4567'), {image: image64})
    Net::HTTP.post_form(URI.parse('http://192.168.0.118:4567'), {image: image64})

    sleep 0.5
end
