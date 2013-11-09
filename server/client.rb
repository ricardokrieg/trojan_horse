require 'base64'
require 'mini_magick'
require 'json'
require 'time'

require './redis_object'

class Client < RedisObject
    attr_accessor :id, :name, :image, :time, :last_activity, :version, :groups

    def self.prefix
        :client
    end

    def set_primary_key(pk)
        @id = pk
    end

    def primary_key
        @id
    end

    def initialize(pk)
        super

        @name = nil
        @image = nil
        @version = nil
        @time = -1
        @last_activity = nil
        @groups = []

    end

    def update(redis_attrs)
        super(redis_attrs)

        begin
            Base64.decode64(@image)

            @time = @time.to_i
            @last_activity = Time.parse(@last_activity)
        rescue Exception => e
            @id = nil

            print 'Exception: '
            puts e.message
        end
    end

    def to_redis
        [:name, @name.to_json, :image, @image.to_json, :last_activity, @last_activity.to_json, :version, @version.to_json, :groups, @groups.to_json]
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
end
