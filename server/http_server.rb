require 'sinatra'
require 'socket'
require 'redis'

require './client'

$redis = Redis.new

get '/' do
    @clients = Client.all

    erb :index
end

get '/favicon.ico' do
end

get '/:id' do
    @client = Client.find(params[:id])

    if @client and @client.active?
        erb :show
    else
        redirect '/'
    end
end

get '/:id/image' do
    @client = Client.find(params[:id])

    if @client
        if @client.active?
            "data:image/jpg;base64,#{@client.image}"
        else
            'redirect'
        end
    end
end

get '/:id/destroy' do
    @client = Client.find(params[:id])

    if @client and not @client.active?
        @client.destroy
    end

    redirect '/'
end
