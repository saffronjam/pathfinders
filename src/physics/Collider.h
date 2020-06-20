#pragma once

#include <optional>

#include <Box2D/Dynamics/b2Body.h>
#include <Box2D/Dynamics/b2Fixture.h>
#include <Box2D/Dynamics/b2World.h>

#include "Lib.h"
#include "VectorLib.h"

class Collider
{
public:
    using Ptr = std::shared_ptr<Collider>;

protected:
    void BuildBase(b2BodyDef *bodyDef, b2FixtureDef *fixtureDef);

public:
    void AssignBody(b2Body *body) noexcept { m_body = body; }

    b2Body *GetBody() const { return m_body.value(); }
    b2Fixture *GetFixture() const { return m_fixture.value(); }

private:
    std::optional<b2Body *> m_body;
    std::optional<b2Fixture *> m_fixture;
};