require 'sinatra'
require 'socket'
require 'redis'

require './client'

$redis = Redis.new

def find_client
    @client = Client.find(params[:id])

    if @client
        return @client
    else
        redirect '/'
    end
end

get '/' do
    @clients = Client.all

    erb :index
end

get '/favicon.ico' do
end

get '/:id' do
    @client = find_client

    erb :show
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
    @client = find_client

    if not @client.active?
        @client.destroy
    end

    redirect '/'
end

get '/:id/manage' do
    @client = find_client

    erb :manage
end

post '/:id/update' do
    @client = find_client

    @client.update({name: params[:name]})
    @client.save

    redirect "/#{params[:id]}/manage"
end
