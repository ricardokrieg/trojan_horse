require 'yaml'

require './client'

class HTTPClient < Client
    attr_accessor :name, :notes

    @current_client_id = 0

    def initialize(id)
        super(id)

        @name = "Client ##{HTTPClient.current_client_id}"
        @notes = ''
    end

    class << self
        def current_client_id
            @current_client_id += 1

            @current_client_id
        end

        def from_send(response)
            params = YAML.load(response)

            return client_from_send(params)
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
                client = self.new(params[:id])

                client.image = params[:image]
                client.last_activity = params[:last_activity]
                client.version = params[:version]

                return client
            end
    end
end
