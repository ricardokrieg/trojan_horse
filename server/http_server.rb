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

configure do
    enable :sessions
end

before '*' do
    session[:message] ||= {}
    session[:message][:success] ||= []
    session[:message][:warning] ||= []
    session[:message][:danger] ||= []
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
    @submit_label = 'Create'

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
    if params[:id].downcase != 'default'
        @group = find_group
        @submit_label = 'Update'

        erb :'groups/edit'
    else
        session[:message][:danger] << "You can't edit Default group"
        redirect '/groups/Default'
    end
end

# create
post '/groups' do
    if params[:name] and params[:name] != '' and not ['default', 'new'].include?(params[:name].downcase)
        @group = Group.new(params[:name])
        @group.save

        session[:message][:success] << 'Group created'
        redirect '/'
    else
        session[:message][:danger] << 'Invalid name'
        redirect '/groups/new'
    end
end

# update
post '/groups/:id/update' do
    @group = find_group

    if params[:name] and params[:name] != '' and not ['default', 'new'].include?(params[:name].downcase)
        @group.update_and_save({name: params[:name]})

        session[:message][:success] << 'Group updated'
        redirect '/'
    else
        session[:message][:danger] << 'Invalid name'
        redirect "/groups/#{@group.primary_key}/edit"
    end
end

# destroy
get '/groups/:id/destroy' do
    if params[:id].downcase != 'default'
        @group = find_group

        @group.destroy

        redirect '/'
    else
        session[:message][:danger] << "You can't remove Default group"
        redirect '/groups/Default'
    end
end

#-------------------------------------------------------------------------------

# show
get '/clients/:id' do
    @client = find_client

    erb :'clients/show'
end

# edit
get '/clients/:id/edit' do
    @client = find_client

    erb :'clients/edit'
end

# update
post '/clients/:id/update' do
    @client = find_client

    @client.update({name: params[:name]})
    @client.save

    redirect "/clients/#{params[:id]}/edit"
end

get '/clients/:id/image' do
    @client = find_client

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
