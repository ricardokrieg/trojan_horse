require 'sinatra'
require 'base64'

require './client'

get '/' do
    @clients = Client.clients
    @clients.each {|c| c.update_thumbnail}

    erb :index

    # image = Client.clients.first.image

    # jquery = "<script src='http://code.jquery.com/jquery-1.10.1.min.js'></script>"
    # script = "<script type='text/javascript'>$(document).ready(function(){setInterval(function(){$.get('/image', function(data){$('img').attr('src', data)})}, 100)});</script>"

    # "#{jquery}#{script}<img src='data:image/jpg;base64,#{image}' />"
end

get '/:id' do
    @client = Client.find_by_id!(params[:id])

    erb :show
end

get '/:id/image' do
    client = Client.find_by_id!(params[:id])

    if client
        "data:image/jpg;base64,#{client.image}"
    end
end

post '/' do
    if params[:id]
        @client = Client.find_by_id(params[:id])

        if params[:image]
            if @client.valid_time?(params)
                image = params[:image].gsub(/-/, '+').gsub(/_/, '/')
                # this is to check if :image contains a valid base64 image
                Base64.decode64(image)

                @client.update(image, params)

                puts params[:time]
            end
        end
    end
end
