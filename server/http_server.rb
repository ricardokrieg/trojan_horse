require 'sinatra'
require 'socket'
require 'redis'
require 'debugger'

require './client'
require './group'

$redis = Redis.new

#-------------------------------------------------------------------------------

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

#-------------------------------------------------------------------------------

get '/' do
    @groups = Group.all

    grouped_clients = @groups.map {|g| g.clients}.flatten.uniq
    all_clients = $redis.keys("client:*").map {|k| k.split(':').last}

    ungrouped_clients = []
    all_clients.each do |client|
        ungrouped_clients << client unless grouped_clients.include?(client)
    end

    if ungrouped_clients.any?
        default_group = Group.new('Default')
        default_group.clients = ungrouped_clients

        @groups.insert(0, default_group)
    end

    erb :admin
end

#-------------------------------------------------------------------------------

# new
get '/groups/new' do
    erb :'groups/new'
end

# show
get '/groups/:id' do
    @group = find_group

    erb :'groups/show'
end

# edit
get '/groups/:id/edit' do
    @group = find_group

    erb :'groups/edit'
end

# create
post '/groups' do
    @group = Group.new(params[:name])
    @group.save

    redirect '/'
end

# update
post '/groups/:id/update' do
end

# destroy
post '/groups/:id/destroy' do
end

#-------------------------------------------------------------------------------

before '/clients/*' do
    @client = find_client
end

# show
get '/clients/:id' do
    erb :show
end

# edit
get '/clients/:id/edit' do
    erb :edit
end

# update
post '/clients/:id/update' do
    @client.update({name: params[:name]})
    @client.save

    redirect "/#{params[:id]}/manage"
end

get '/clients/:id/image' do
    "data:image/jpg;base64,#{@client.image}"
end

# get '/clients/:id/destroy' do
#     @client = find_client

#     if not @client.active?
#         @client.destroy
#     end

#     redirect '/'
# end

#-------------------------------------------------------------------------------
