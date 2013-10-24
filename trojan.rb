require 'base64'
require 'net/http'
require 'rbconfig'
require 'open3'

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
        filename = Dir.entries(Dir::pwd).sort_by {|f| File.mtime(f)}.last
        image = nil
        image = Base64.encode64(open(filename, 'rb') {|io| io.read}) if filename.start_with?('_')

        {format: 'jpg', image: image}
    else
        require 'RMagick'

        image = Magick::Image.capture {self.filename = 'root'}
        image.format = 'jpg'

        {format: 'jpg', image: Base64.encode64(image.to_blob)}
    end
end

if os == :windows
    Open3.popen3(File.join(File.dirname($0), "screen_capture.exe"))
end

loop do
    begin
        params = capture

        Net::HTTP.post_form(URI.parse('http://192.168.0.118:4567'), params) unless params[:image].nil?

        sleep 0.2
    rescue Exception => e
    end
end
