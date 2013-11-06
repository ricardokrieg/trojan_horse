require 'yaml'
require 'debugger'

require './client'

class HTTPClient < Client
    class << self
        def from_send(response)
            return client_from_send(response)
        end

        def multiple_from_send(response)
            clients = []

            YAML.load(response).each do |params|
                clients << client_from_send(params)
            end

            return clients
        end

        private
            def client_from_send(params)
                params = YAML.load(params)

                if params
                    client = self.new(params[:id])

                    client.image = params[:image]
                    client.last_activity = params[:last_activity]
                    client.version = params[:version]

                    return client
                else
                    return nil
                end
            end
    end
end
