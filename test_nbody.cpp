#include "perfmon.h"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>


class Point {
public:
    Point(double x, double y, double z)
      : x_(x), y_(y), z_(z)
    { }

    const Point& operator+ () const
    {
        return *this;
    }

    Point operator- () const
    {
        return Point(-x_, -y_, -z_);
    }

    Point& operator+= (const Point& point)
    {
        x_ += point.x_;
        y_ += point.y_;
        z_ += point.z_;
        return *this;
    }

    Point& operator-= (const Point& point)
    {
        x_ -= point.x_;
        y_ -= point.y_;
        z_ -= point.z_;
        return *this;
    }

    Point& operator*= (double c)
    {
        x_ *= c;
        y_ *= c;
        z_ *= c;
        return *this;
    }

    Point& operator/= (double c)
    {
        x_ /= c;
        y_ /= c;
        z_ /= c;
        return *this;
    }

    double Length2() const
    {
        return x_ * x_ + y_ * y_ + z_ * z_;
    }

    double Length() const
    {
        return std::sqrt(Length2());
    }

private:
    double x_;
    double y_;
    double z_;
};

Point operator+ (const Point& left, const Point& right)
{
    return Point(left) += right;
}

Point operator- (const Point& left, const Point& right)
{
    return Point(left) -= right;
}

Point operator* (const Point& left, double right)
{
    return Point(left) *= right;
}

Point operator/ (const Point& left, double right)
{
    return Point(left) /= right;
}

Point operator* (double left, const Point& right)
{
    return Point(right) *= left;
}


struct Body {
    double mass;
    Point position;
    Point velocity;
};

void Advance(std::vector<Body>* const bodies, double dt)
{
    for (size_t i = 1; i < bodies->size(); ++i) {
        auto& i_body = (*bodies)[i];
        for (size_t j = 0; j < i; ++j) {
            auto& j_body = (*bodies)[j];

            const Point direction = i_body.position - j_body.position;
            const double distance2 = direction.Length2();
            const double mag = dt / distance2 / std::sqrt(distance2);

            i_body.velocity -= direction * mag * j_body.mass;
            j_body.velocity += direction * mag * i_body.mass;
        }
    }
    for (Body& body : *bodies) {
        body.position += body.velocity * dt;
    }
}

double Energy(const std::vector<Body>& bodies)
{
    double energy = 0.0;
    for (size_t i = 1; i < bodies.size(); ++i) {
        auto& i_body = bodies[i];
        energy += 0.5 * i_body.mass * i_body.velocity.Length2();
        for (size_t j = 0; j < i; ++j) {
            auto& j_body = bodies[j];
            energy -= i_body.mass * j_body.mass / (i_body.position - j_body.position).Length();
        }
    }
    return energy;
}

void OffsetSolarMomentum(std::vector<Body>* planets)
{
    Point momentum(0.0, 0.0, 0.0);
    for (const auto& planet : *planets) {
        momentum += planet.velocity * planet.mass;
    }
    planets->at(0).velocity = - momentum / planets->at(0).mass;
}

// /**
//  * Данная функция реализует шаг метода Рунги-Кутта четвёртого порядка
//  */
// template <typename Function, typename Value>
// Value RungeKuttaMethodStep(const Function& function, const Value& y, const Real t, const Real h)
// {
//     const auto k1 = h * function(t, y);
//     const auto k2 = h * function(t + h/2, y + k1/2);
//     const auto k3 = h * function(t + h/2, y + k2/2);
//     const auto k4 = h * function(t + h, y + k3);
//     return y + (k1 + 2*k2 + 2*k3 + k4) / 6;
// }


int main()
{
    const double pi = 4.0 * std::atan(1.0);
    const double solar_mass = 4.0 * pi * pi;
    const double days_per_year = 365.24;

    std::vector<Body> bodies = {
        { /* sun */
            solar_mass,
            { 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0 },
        },

        { /* jupiter */
            9.54791938424326609e-04 * solar_mass,
            {
                4.84143144246472090e+00,
               -1.16032004402742839e+00,
               -1.03622044471123109e-01
            },
            {
                1.66007664274403694e-03 * days_per_year,
                7.69901118419740425e-03 * days_per_year,
               -6.90460016972063023e-05 * days_per_year
            }
        },

        { /* saturn */
            2.85885980666130812e-04 * solar_mass,

            {
                8.34336671824457987e+00,
                4.12479856412430479e+00,
               -4.03523417114321381e-01
            },
            {
                -2.76742510726862411e-03 * days_per_year,
                4.99852801234917238e-03 * days_per_year,
                2.30417297573763929e-05 * days_per_year
            }
        },

        { /* uranus */
            4.36624404335156298e-05 * solar_mass,
            {
                1.28943695621391310e+01,
                -1.51111514016986312e+01,
                -2.23307578892655734e-01
            },
            {
                2.96460137564761618e-03 * days_per_year,
                2.37847173959480950e-03 * days_per_year,
                -2.96589568540237556e-05 * days_per_year
            }
        },

        { /* neptune */
            5.15138902046611451e-05 * solar_mass,
            {
                1.53796971148509165e+01,
                -2.59193146099879641e+01,
                1.79258772950371181e-01
            },
            {
                2.68067772490389322e-03 * days_per_year,
                1.62824170038242295e-03 * days_per_year,
                -9.51592254519715870e-05 * days_per_year
            }
        }
    };
    OffsetSolarMomentum(&bodies);

    std::cout << std::setprecision(9) << Energy(bodies) << std::endl;

    int n = 5000000;
    for (int i = 0; i < n; ++i) {
        Advance(&bodies, 1e-2);
    }

    std::cout << std::setprecision(9) << Energy(bodies) << std::endl;
    return 0;
}
