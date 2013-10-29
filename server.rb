require 'sinatra'
require 'base64'

configure do
    @@image = nil
    @@time = -1
end

get '/' do
    jquery = "<script src='http://code.jquery.com/jquery-1.10.1.min.js'></script>"
    script = "<script type='text/javascript'>$(document).ready(function(){setInterval(function(){$.get('/image', function(data){$('img').attr('src', data)})}, 100)});</script>"
    "#{jquery}#{script}<img src='data:image/jpg;base64,#{@@image}' />"
end

get '/image' do
    "data:image/jpg;base64,#{@@image}"
end

post '/' do
    # puts "#{@@time}   --   #{params[:time]}   --   #{params[:time].to_i}"

    if params[:image]
        # what happens if user change system time to a past date? It stop working =P
        if params[:time].to_i > @@time
            image = params[:image].gsub(/-/, '+').gsub(/_/, '/')

            # puts image

            Base64.decode64(image)

            @@image = image
            @@time = params[:time].to_i
        end
    end
    # else
    #     image = Magick::Image.from_blob(Base64.decode64(params[:image])) {|img| img.format = 'jpg'}.first
    #     @@image = Base64.encode64(image.to_blob)
end
