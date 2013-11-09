require 'socket'
require 'redis'
require 'base64'
require 'json'

require './client'

# Thread.abort_on_exception = true

$tcp_server = TCPServer.new 61400
$redis = Redis.new

def recent?(id, time)
    client = Client.find(id)

    if client
        return client.recent?(time)
    else
        return true
    end
end

def parse_message(message)
    params = message.split('&')

    image = params[0].split('=').last
    id = params[1].split('=').last
    time = params[2].split('=').last
    version = params[3].split('=').last
    last_activity = Time.now

    begin
        Base64.decode64(image)

        id.gsub!(/\//, '')
        time = time.to_i

        if recent?(id, time)
            to_redis = {image: image, time: time, version: version, last_activity: last_activity}.to_json

            $redis.set(id, to_redis)
        end
    rescue Exception => e
        print 'Exception: '
        puts e.message
    end
end

loop do
    puts 'Waiting Client...'
    Thread.start($tcp_server.accept) do |tcp_connection|
        puts 'Connected to Client'

        last_message = Time.now

        while message = tcp_connection.gets
            parse_message(message)

            puts "#{(Time.now - last_message).round(2)}s"
            last_message = Time.now
        end

        puts 'Closing Client connection...'
        tcp_connection.close
    end
end
