require 'sinatra'
require 'base64'
require 'socket'
require 'mini_magick'
require 'debugger'

require './http_client'

configure do
    @@tcp_server = TCPSocket.new 'localhost', '61401'
end

get '/' do
    @@tcp_server.puts '0'
    response = ''
    while line = @@tcp_server.gets
        break if line == "<end>\n"

        response += line
    end

    @clients = HTTPClient.multiple_from_send(response)

    # begin
    #     c = clients.first
    #     i = Base64.urlsafe_decode64(c.image)
    #     i = Base64.encode64(i)
    #     c.image = i
    # rescue
    # end

    erb :index
end

get '/:id' do
    @client = HTTPClient.find_by_id!(params[:id])

    if @client
        erb :show
    end
end

get '/:id/image' do
    @client = HTTPClient.find_by_id!(params[:id])

    if @client
        "data:image/jpg;base64,#{@client.image}"
    end
end

# post '/' do
#     if params[:id]
#         @client = Client.find_by_id(params[:id])

#         if params[:image]
#             if @client.valid_time?(params)
#                 image = params[:image].gsub(/-/, '+').gsub(/_/, '/')
#                 # Base64.decode64(image)

#                 @client.update(image, params)

#                 puts params[:time]
#             end
#         end
#     end
# end
