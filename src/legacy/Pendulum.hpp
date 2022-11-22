#pragma once

#include "Box.hpp"

class PendulumComponent final : public Box
{
public:
  PendulumComponent(double _gravity, double _length, double _theta, double _mass)
     : Box(glm::vec3(0.1, 0.1, _length)),
       root_angular_velocity(_gravity / _length),
       mass(_mass),
       previous({_theta, 1.0})
  {
  }

protected:
  void beforeUpdate(Renderer* renderer, UpdateData& data) override
  {
    previous.second -= root_angular_velocity * glm::sin(previous.first) * data.dt;
    previous.first += previous.second * data.dt;

    m_localToParent = glm::rotate((float)previous.first, glm::vec3(-1.0, 0.0, 0.0));
    m_localToParent = glm::translate(m_localToParent, glm::vec3(0.0, 0.0, -m_scale.z * 0.5));

    data.localToWorld = data.parentToWorld * m_localToParent;
    Box::beforeUpdate(renderer, data);
  }
  
private:
 double root_angular_velocity;
 double mass;

 std::pair<double, double> previous;
};