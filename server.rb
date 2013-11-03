require 'sinatra'
require 'base64'

class Client
    attr_reader :id
    attr_accessor :image, :time

    @clients = []

    def initialize(id)
        @id = id

        @image = nil
        @time = -1
    end

    def self.find_by_id(id)
        client = @clients.select {|c| c.id == id}.first

        if client.nil?
            client = Client.new(id)
            @clients << client
        end

        return client
    end

    def self.clients
        @clients
    end
end

# configure do
#     @@clients = []
# end

get '/' do
    # image = @@clients.first.image
    image = Client.clients.first.image

    jquery = "<script src='http://code.jquery.com/jquery-1.10.1.min.js'></script>"
    script = "<script type='text/javascript'>$(document).ready(function(){setInterval(function(){$.get('/image', function(data){$('img').attr('src', data)})}, 100)});</script>"

    "#{jquery}#{script}<img src='data:image/jpg;base64,#{image}' />"
end

get '/image' do
    client = Client.clients.first

    if client
        image = client.image

        "data:image/jpg;base64,#{image}"
    end
end

post '/' do
    if params[:id]
        client = Client.find_by_id(params[:id])

        if params[:image]
            if params[:time].to_i >= client.time or (client.time - params[:time].to_i) > 60
                image = params[:image].gsub(/-/, '+').gsub(/_/, '/')

                # this is to valida :image contains a valid base64 image
                Base64.decode64(image)

                client.image = image
                client.time = params[:time].to_i
            end
        end
    end
end
