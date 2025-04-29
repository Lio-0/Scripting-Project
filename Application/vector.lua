-- vector.lua

local vector = {}
vector.__index = vector

function vector.new(x, y, z)
    local t = {
        x = x or 0,
        y = y or 0,
        z = z or 0
    }
    return setmetatable(t, vector)
end

function isvector(t)
    return getmetatable(t) == vector
end

function vector.__newindex(t, k, v)
    print("vector - not possible to assign new fields")
end

function vector.__tostring(t)
    return "(" .. t.x .. ", " .. t.y .. ", " .. t.z .. ")"
end

function vector.__unm(t)
    return vector.new(-t.x, -t.y, -t.z)
end

function vector.__add(a, b)
    assert(isvector(a) and isvector(b), "vector add - expected args: vector, vector")
    return vector.new(a.x + b.x, a.y + b.y, a.z + b.z)
end

function vector.__sub(a, b)
    assert(isvector(a) and isvector(b), "vector sub - expected args: vector, vector")
    return vector.new(a.x - b.x, a.y - b.y, a.z - b.z)
end

function vector.__mul(a, b)
    if type(a) == "number" then
        return vector.new(a * b.x, a * b.y, a * b.z)
    elseif type(b) == "number" then
        return vector.new(a.x * b, a.y * b, a.z * b)
    else
        assert(isvector(a) and isvector(b), "vector mul - expected args: vector or number")
        return vector.new(a.x * b.x, a.y * b.y, a.z * b.z)
    end
end

function vector.__div(a, b)
    assert(isvector(a), "vector div - expected args: vector, vector or number")
    if type(b) == "number" then
        return vector.new(a.x / b, a.y / b, a.z / b)
    else
        assert(isvector(b), "vector div - expected args: vector, vector or number")
        return vector.new(a.x / b.x, a.y / b.y, a.z / b.z)
    end
end

function vector.__eq(a, b)
    assert(isvector(a) and isvector(b), "vector eq - expected args: vector, vector")
    return a.x == b.x and a.y == b.y and a.z == b.z
end

function vector:length()
    return math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
end

function vector:normalize()
    return self/self.length
end

return setmetatable(vector, {
    __call = function(_, ...)
        return vector.new(...)
    end
})
