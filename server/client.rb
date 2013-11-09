require 'base64'
require 'mini_magick'
require 'json'
require 'time'

class Client
    attr_reader :id
    attr_accessor :image, :last_activity, :version, :groups

    def initialize(id, redis_attrs)
        @id = id

        begin
            attrs = JSON.parse(redis_attrs)

            image = attrs['image']
            Base64.decode64(image)

            @time = attrs['time'].to_i
            @image = attrs['image']
            @version = attrs['version']
            @last_activity = Time.parse(attrs['last_activity'])

            @groups = attrs['groups']
            @groups ||= []
        rescue Exception => e
            @id = nil

            print 'Exception: '
            puts e.message
        end
    end

    def save
        $redis.set(@id, {groups: @groups}.to_json) == 'OK'
    end

    def recent?(time)
        time >= @time or (@time - time) > 60
    end

    def active?
        (Time.now - @last_activity).to_i < 30
    end

    def thumbnail
        decoded_image = Base64.decode64(@image)
        image = MiniMagick::Image.read(decoded_image)

        image.resize('400x300')
        image.colorspace('Gray') unless active?

        Base64.encode64(image.to_blob)
    end


    def destroy
        $redis.del(@id)
    end

    class << self
        def all
            clients = []

            $redis.keys.each do |id|
                if client = find(id)
                    clients << client
                end
            end

            return clients
        end

        def find(id)
            if client = $redis.get(id)
                client = new(id, client)

                if client.id.nil?
                    return nil
                else
                    return client
                end
            else
                return nil
            end
        end
    end
end
