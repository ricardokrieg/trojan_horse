require 'debugger'

require './client'

class TCPClient < Client
    attr_accessor :time

    def recent?(time)
        time = time.to_i

        time >= @time or (@time - time) > 60
    end

    class << self
        def parse_message(clients, message)
            params = message.split('&')

            image = params[0].split('=').last
            id = params[1].split('=').last
            time = params[2].split('=').last
            version = params[3].split('=').last

            client = find_by_id!(clients, id)
            client.update(image, time, version)

            puts time
        end

        def clients_to_send(clients)
            message = clients.map {|client| client.to_send}.to_yaml
            message += "<end>\n"

            return message
        end

        def client_to_send(clients, id)
            client = find_by_id(clients, id)

            if client
                message = client.to_send
                message += "<end>\n"

                message
            else
                "<end>\n"
            end
        end
    end
end
