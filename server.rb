require 'sinatra'

configure do
    @@image = nil
end

get '/' do
    # filename = Time.now.to_f.to_s.gsub(/\./, '')
    # Image.from_blob(Base64.urlsafe_decode64(@@image)).first.write('screens/image.jpg')

    jquery = "<script src='http://code.jquery.com/jquery-1.10.1.min.js'></script>"
    script = "<script type='text/javascript'>$(document).ready(function(){setInterval(function(){$.get('/image', function(data){$('img').attr('src', data)})}, 100)});</script>"
    "#{jquery}#{script}<img src='data:image/jpeg;base64,#{@@image}' />"
end

get '/image' do
    "data:image/jpeg;base64,#{@@image}"
end

post '/' do
    @@image = params[:image]
end
