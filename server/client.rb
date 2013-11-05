require 'mini_magick'
require 'base64'

class Client
    attr_reader :id
    attr_accessor :image, :thumbnail, :time, :last_activity, :name, :notes, :version

    @clients = []
    @current_client_id = 0

    def initialize(id)
        @id = id

        @image = nil
        @thumbnail = nil
        @time = -1
        @last_activity = nil
        @name = "Client ##{Client.current_client_id}"
        @notes = ''
        @version = 0
    end

    def update(image, params)
        @image = image
        @time = params[:time].to_i

        @last_activity = Time.now

        thumb = MiniMagick::Image.read(Base64.decode64(image))
        thumb.resize('400x300')
        thumb.colorspace('Gray') unless active?
        @thumbnail = Base64.encode64(thumb.to_blob)
    end

    def update_thumbnail
        thumb = MiniMagick::Image.read(Base64.decode64(@thumbnail))
        thumb.colorspace('Gray') unless active?
        @thumbnail = Base64.encode64(thumb.to_blob)
    end

    def valid_time?(params)
        params[:time].to_i >= @time or (@time - params[:time].to_i) > 60
    end

    def active?
        !@last_activity.nil? and (Time.now - @last_activity).to_i < 60
    end

    class << self
        def find_by_id(id)
            client = self.find_by_id!(id)

            if client.nil?
                client = Client.new(id)
                @clients << client
            end

            return client
        end

        def find_by_id!(id)
            @clients.select {|c| c.id == id}.first
        end

        def clients
            @clients
        end

        def current_client_id
            @current_client_id += 1

            @current_client_id
        end
    end
end
