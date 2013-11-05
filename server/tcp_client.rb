require 'yaml'

require './client'

class TCPClient < Client
    attr_accessor :time

    def recent?(time)
        time = time.to_i

        time >= @time or (@time - time) > 60
    end

    def to_send
        {id: @id, image: @image, last_activity: @last_activity}.to_yaml
    end

    class << self
        def parse_message(message)
            params = message.split('&')

            image = params[0].split('=').last
            id = params[1].split('=').last
            time = params[2].split('=').last
            version = params[3].split('=').last

            client = find_by_id(id)
            client.update(image, time, version)

            # base64_image = Base64.urlsafe_decode64(image)
        end

        def clients_to_send
            @clients.map {|client| client.to_send}.to_yaml
        end

        def client_to_send(id)
            client = find_by_id!(id)

            if client
                client.to_send
            else
                nil
            end
        end
    end
end
