local transform = {}
transform.__index = transform

local vector = require("vector")

local function new(p, r, s)
    assert(p == nil or vector.isvector(p), "transform new - expected args: vector or nil")
    assert(r == nil or vector.isvector(r), "transform new - expected args: vector or nil")
    assert(s == nil or vector.isvector(s), "transform new - expected args: vector or nil")

    local t = {
        position = p or vector(),
        rotation = r or vector(),
        scale = s or vector()
    }
    return setmetatable(t, transform)
end

local function istransform(t)
    return getmetatable(t) == transform
end

function transform.__newindex(_, k, _)
    print("transform - not possible to assign new fields: " .. tostring(k))
end

function transform:__tostring()
    return "position: " .. tostring(self.position) ..
           ", rotation: " .. tostring(self.rotation) ..
           ", scale: " .. tostring(self.scale)
end

function transform.__eq(a, b)
    assert(istransform(a) and istransform(b), "transform eq - expected args: transform, transform")
    return a.position == b.position and a.rotation == b.rotation and a.scale == b.scale
end

transform.new = new
transform.istransform = istransform

return setmetatable(transform, {
    __call = function(_, ...)
        return new(...)
    end
})
