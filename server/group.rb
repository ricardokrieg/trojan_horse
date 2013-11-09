require 'json'

require './redis_object'

class Group < RedisObject
    attr_accessor :name, :clients

    def self.prefix
        :group
    end

    def set_primary_key(pk)
        @name = pk
    end

    def primary_key
        @name
    end

    def initialize(pk)
        super

        @clients = []
    end

    def to_redis
        [:clients, @clients.to_json]
    end
end
