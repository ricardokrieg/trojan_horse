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

use Rack::Auth::Basic, 'Restricted Area' do |username, password|
    username.downcase == 'admin' and password.downcase == 'x2013'
end

configure do
    enable :sessions
end

before '*' do
    session[:message] ||= {}
    session[:message][:success] ||= []
    session[:message][:warning] ||= []
    session[:message][:danger] ||= []
end

get %r{(/.*[^\/])$} do
    redirect "#{params[:captures].first}/"
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
get '/groups/new/' do
    @submit_label = 'Create'
    @cancel_path = '/'

    erb :'groups/new'
end

# show
get '/groups/:id/' do
    session[:client_back] = request.path_info
    session[:client_edit_back] = request.path_info

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
get '/groups/:id/edit/' do
    if params[:id].downcase != 'default'
        @group = find_group
        @submit_label = 'Update'
        @cancel_path = "/groups/#{params[:id]}"

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

    if params[:name] and params[:name] != '' and not ['default', 'new', 'edit', 'destroy'].include?(params[:name].downcase)
        @group.update_and_save({name: params[:name]})

        session[:message][:success] << 'Group updated'
        redirect '/'
    else
        session[:message][:danger] << 'Invalid name'
        redirect "/groups/#{@group.primary_key}/edit"
    end
end

# destroy
get '/groups/:id/destroy/' do
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
get '/clients/:id/' do
    session[:client_back] ||= '/'
    session[:client_edit_back] = request.path_info

    @client = find_client

    erb :'clients/show'
end

# edit
get '/clients/:id/edit/' do
    session[:client_edit_back] ||= "/clients/#{params[:id]}"

    @client = find_client
    @groups = $redis.keys("group:*").map {|g| g.split(':').last}.map {|g| [@client.groups.include?(g), g]}

    erb :'clients/edit'
end

# update
post '/clients/:id/update' do
    @client = find_client

    if not ['edit', 'image'].include?(params[:name].downcase)
        @client.update({name: params[:name], groups: params[:groups].to_s})
        @client.save

        session[:message][:success] << 'Client updated'
        redirect "/clients/#{params[:id]}"
    else
        session[:message][:danger] << 'Invalid name'
        redirect "/clients/#{@client.primary_key}/edit"
    end
end

get '/clients/:id/image/' do
    @client = find_client

    "data:image/jpg;base64,#{@client.image}"
end

#-------------------------------------------------------------------------------
