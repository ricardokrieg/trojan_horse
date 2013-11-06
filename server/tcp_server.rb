require 'socket'

require './tcp_client'

Thread.abort_on_exception=true

tcp_server = TCPServer.new 61400
http_server = TCPServer.new 61401

Thread.start do
    puts 'Waiting HTTP Server...'

    http_connection = http_server.accept

    puts 'Connected to HTTP Server'

    while id = http_connection.gets
        if id == "0\n"
            to_send = TCPClient.clients_to_send
        else
            to_send = TCPClient.client_to_send(id)
        end

        http_connection.puts to_send
    end

    puts 'Closing HTTP connection'
end

loop do
    puts 'Waiting Client...'
    Thread.start(tcp_server.accept) do |tcp_connection|
        puts 'Connected to Client'

        last_message = Time.now

        while message = tcp_connection.gets
            TCPClient.parse_message(message)

            puts "#{(Time.now - last_message).round(2)}s"
            last_message = Time.now
        end

        puts 'Closing Client connection...'
        tcp_connection.close
    end
end
