require 'sinatra'
require 'socket'
require 'redis'

require './client'
require './group'

$redis = Redis.new

def find_object(klass)
    @object = klass.find(params[:id])

    if @object
        return @object
    else
        redirect '/'
    end
end

def find_client
    find_object(Client)
end

def find_group
    find_object(Group)
end

get '/' do
    @clients = Client.all

    erb :index
end

get '/favicon.ico' do
end

get '/groups' do
    @groups = Group.all

    erb :groups
end

get '/new-group' do
    erb :new_group
end

get '/group/:id' do
    @group = find_group

    erb :group
end

post '/groups' do
    @group = Group.new(params[:name])
    @group.save

    redirect '/groups'
end

get '/:id' do
    @client = find_client

    erb :show
end

get '/:id/image' do
    @client = find_client

    "data:image/jpg;base64,#{@client.image}"
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
