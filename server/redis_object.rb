require 'json'

class RedisObject
    def initialize(pk)
        set_primary_key(pk)
    end

    def save
        $redis.hmset(redis_key, *to_redis) == 'OK'
    end

    def update(redis_attrs)
        redis_attrs.each do |key, value|
            begin
                instance_variable_set("@#{key}", JSON.parse(value))
            rescue JSON::ParserError
                instance_variable_set("@#{key}", value)
            end
        end
    end

    def update_and_save(redis_attrs)
        destroy
        update(redis_attrs)
        save
    end

    def destroy
        $redis.del(redis_key)
    end

    def prefix
        self.class.prefix
    end

    def redis_key
        "#{prefix}:#{primary_key}"
    end

    class << self
        def all
            objects = []

            $redis.keys("#{prefix}:*").each do |key|
                if redis_object = find(key, false)
                    objects << redis_object if redis_object.primary_key != nil
                end
            end

            return objects
        end

        def find(pk, use_prefix=true)
            if use_prefix
                redis_key = "#{prefix}:#{pk}"
            else
                redis_key = pk
            end

            if $redis.hlen(redis_key) > 0
                redis_attrs = $redis.hgetall(redis_key)

                if use_prefix
                    redis_object = new(pk)
                else
                    i = pk.index(':')
                    size = pk.size
                    redis_object = new(pk[i+1, size-1])
                end
                redis_object.update(redis_attrs)

                return redis_object
            else
                return nil
            end
        end
    end
end
