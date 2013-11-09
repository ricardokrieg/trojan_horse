require 'socket'
require 'redis'
require 'base64'
require 'json'

# Thread.abort_on_exception = true

$tcp_server = TCPServer.new 61400
$redis = Redis.new

loop do
    puts 'Waiting Client...'
    Thread.start($tcp_server.accept) do |tcp_connection|
        puts 'Connected to Client'

        last_message = Time.now
        client = nil
        message = nil

        while message = tcp_connection.gets
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
                last_time = $redis.hget("client:#{id}", :time).to_i

                if time >= last_time or (last_time - time) > 60
                    to_redis = [:image, image, :last_activity, last_activity.to_json, :time, time, :version, version]
                    $redis.hmset("client:#{id}", *to_redis)
                end
            rescue Exception => e
                print 'Exception: '
                puts e.message
            end

            puts "#{(Time.now - last_message).round(2)}s"
            last_message = Time.now
        end

        puts 'Closing Client connection...'
        tcp_connection.close
    end
end
