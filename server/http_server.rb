require 'sinatra'
require 'socket'

require './http_client'

Thread.abort_on_exception = true
$semaphore = Mutex.new

$tcp_server = nil

Thread.start do
    loop do
        begin
            raise Errno::ECONNREFUSED if $tcp_server.nil?

            $semaphore.synchronize {
                $tcp_server.puts 'ping'
                $tcp_server.gets
            }

            sleep 1
        rescue Errno::EPIPE, Errno::ECONNREFUSED
            $semaphore.synchronize {
                $tcp_server = nil
            }

            puts 'Connecting to TCP Server...'
            loop do
                begin
                    $semaphore.synchronize {
                        $tcp_server = TCPSocket.new 'localhost', '61401'
                    }
                    break
                rescue Exception => e
                    puts e.message
                    sleep 1
                end
            end
            puts 'Connected to TCP Server'
        end
    end
end

def connect(message)
    loop do
        if $tcp_server.nil?
            sleep 0.5
        else
            $semaphore.synchronize {
                $tcp_server.puts message

                response = ''
                while line = $tcp_server.gets
                    break if line == "<end>\n"

                    response += line
                end

                return response
            }
        end
    end
end

get '/' do
    tcp_response = connect('all')

    @clients = HTTPClient.multiple_from_send(tcp_response)

    erb :index
end

get '/favicon.ico' do
end

get '/:id' do
    tcp_response = connect(params[:id])

    @client = HTTPClient.from_send(tcp_response)

    if @client
        erb :show
    end
end

get '/:id/image' do
    tcp_response = connect(params[:id])

    @client = HTTPClient.from_send(tcp_response)

    if @client
        "data:image/jpg;base64,#{@client.image}"
    end
end
