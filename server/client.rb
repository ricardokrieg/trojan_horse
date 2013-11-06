require 'yaml'
require 'base64'
require 'mini_magick'

class Client
    attr_reader :id
    attr_accessor :image, :last_activity, :version

    def initialize(id)
        @id = id

        update(nil, -1, 0)
    end

    def update(image, time, version)
        begin
            base64_image = Base64.decode64(image)

            # magick_image = MiniMagick::Image.read(base64_image)
            # magick_image.resize('50%')

            # image = Base64.encode64(magick_image.to_blob)

            @image = image
            @time = time.to_i
            @version = version

            @last_activity = Time.now
        rescue Exception => e
            print 'Exception: '
            puts e.message
        end
    end

    def thumbnail
        decoded_image = Base64.decode64(@image)
        image = MiniMagick::Image.read(decoded_image)

        image.resize('400x300')
        image.colorspace('Gray') unless active?

        Base64.encode64(image.to_blob)
    end

    def active?
        !@last_activity.nil? and (Time.now - @last_activity).to_i < 60
    end

    def to_send
        {id: @id, image: @image, last_activity: @last_activity}.to_yaml
    end

    class << self
        def find_by_id(clients, id)
            clients.select {|c| c.id == id}.first
        end

        def find_by_id!(clients, id)
            client = find_by_id(clients, id)

            if client.nil?
                client = new(id)
                clients << client
            end

            return client
        end
    end
end
