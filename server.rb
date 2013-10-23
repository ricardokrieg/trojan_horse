require 'sinatra'
require 'base64'
require 'RMagick'

configure do
    @@image = nil
end

get '/' do
    # filename = Time.now.to_f.to_s.gsub(/\./, '')
    # Image.from_blob(Base64.urlsafe_decode64(@@image)).first.write('screens/image.jpg')

    jquery = "<script src='http://code.jquery.com/jquery-1.10.1.min.js'></script>"
    script = "<script type='text/javascript'>$(document).ready(function(){setInterval(function(){$.get('/image', function(data){$('img').attr('src', data)})}, 100)});</script>"
    "#{jquery}#{script}<img src='data:image/jpg;base64,#{@@image}' />"
end

get '/image' do
    "data:image/jpg;base64,#{@@image}"
end

post '/' do
    if params[:format] == 'jpg'
        @@image = params[:image]
    else
        image = Magick::Image.from_blob(Base64.decode64(params[:image])) {|img| img.format = 'jpg'}.first
        @@image = Base64.encode64(image.to_blob)

        # open('from_windows.bmp', 'w') {|io| io.write(Base64.decode64(params[:image]))}
        # image = Magick::Image.from_blob(Base64.decode64(params[:image])) {|img| img.format = 'bmp'}.first
    end
end
