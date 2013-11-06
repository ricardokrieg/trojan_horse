require 'socket'

require './tcp_client'

# Thread.abort_on_exception = true

$tcp_server = TCPServer.new 61400
$http_server = TCPServer.new 61401

$clients = []

Thread.start do
    loop do
        puts 'Waiting HTTP Server...'

        http_connection = $http_server.accept

        puts 'Connected to HTTP Server'

        while message = http_connection.gets
            if message == "ping\n"
                to_send = "pong\n"
            elsif message == "all\n"
                to_send = TCPClient.clients_to_send($clients)
            elsif message.start_with?("DESTROY:")
                id = message[8..-2]
                puts "Destroy: #{id}"

                TCPClient.destroy!($clients, id)
                to_send = "<end>\n"
            else
                id = message[0..-2]
                to_send = TCPClient.client_to_send($clients, id)
            end

            http_connection.puts to_send
        end

        puts 'Closing HTTP connection'
    end
end

loop do
    puts 'Waiting Client...'
    Thread.start($tcp_server.accept) do |tcp_connection|
        puts 'Connected to Client'

        last_message = Time.now

        while message = tcp_connection.gets
            TCPClient.parse_message($clients, message)

            puts "#{(Time.now - last_message).round(2)}s"
            last_message = Time.now
        end

        puts 'Closing Client connection...'
        tcp_connection.close
    end
end
