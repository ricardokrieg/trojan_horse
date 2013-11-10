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

    ungrouped_clients = Client.ungrouped(@groups)

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
    @group = Group.find(params[:id])

    if not @group
        groups = Group.all
        ungrouped_clients = Client.ungrouped(groups)

        if ungrouped_clients.any?
            @group = Group.new('Default')
            @group.clients = ungrouped_clients
        else
            redirect '/'
        end
    end

    @clients = @group.clients.map {|c| Client.find(c)}
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

# before '/clients/*' do
#     @client = find_client
# end

# show
get '/clients/:id' do
    @client = find_client

    erb :'clients/show'
end

# edit
get '/clients/:id/edit' do
    erb :'clients/edit'
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
