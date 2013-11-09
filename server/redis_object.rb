class RedisObject
    def initialize(pk)
        set_primary_key(pk)
    end

    def save
        $redis.hmset(redis_key, *to_redis) == 'OK'
    end

    def update(redis_attrs)
        redis_attrs.each do |key, value|
            instance_variable_set("@#{key}", value)
        end
    end

    def prefix
        self.class.prefix
    end

    def redis_key
        "#{prefix}:#{primary_key}"
    end

    class << self
        def find(pk)
            redis_key = "#{prefix}:#{pk}"

            if $redis.hlen(redis_key) > 0
                redis_attrs = $redis.hgetall(redis_key)

                redis_object = new(pk)
                redis_object.update(redis_attrs)

                return redis_object
            else
                return nil
            end
        end
    end
end
