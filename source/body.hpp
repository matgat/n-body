#pragma once
//  ---------------------------------------------
//  A spherical body for n-body simulation
//  ---------------------------------------------
#include <stdexcept>
#include <cmath> // std::log10, std::cbrt
#include <numbers> // std::numbers::pi
#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
template<class Vect> class SphericalBody final
{
 public:
    SphericalBody(const double m, const Vect& pos, const Vect& spd)
      : i_mass(m)
      , i_radius(calc_radius_from_mass(i_mass))
      , i_pos(pos)
      , i_spd(spd)
       {
        if( math::is_zero(i_mass) )
           {
            throw std::runtime_error("Mass cannot be null");
           }
       }

    [[nodiscard]] static double calc_radius_from_mass(const double m) noexcept
       {
        //return std::log10(1.0 + m);
        return std::cbrt(m/(density() * (4.0/3.0) * std::numbers::pi));
       }

    [[nodiscard]] double mass() const noexcept { return i_mass; }
    [[nodiscard]] double radius() const noexcept { return i_radius; }
    [[nodiscard]] static double density() noexcept { return 1.0; }
    [[nodiscard]] double volume() const noexcept { return (4.0/3.0) * std::numbers::pi * math::cube(i_radius); }

    [[nodiscard]] const Vect& position() const noexcept { return i_pos; }
    [[nodiscard]] const Vect& speed() const noexcept { return i_spd; }
    [[nodiscard]] const Vect& acceleration() const noexcept { return i_acc; }

    void set_speed(const Vect new_spd) noexcept { i_spd = new_spd; }

    [[nodiscard]] Vect displacement_from(const SphericalBody& other) const noexcept
       {
        return position() - other.position();
       }

    //[[nodiscard]] bool collides_with(const SphericalBody& other) const noexcept
    //   {
    //    return displacement_from(other).norm() < (radius() + other.radius());
    //   }

    [[nodiscard]] double kinetic_energy() const noexcept
       {// ½ m·V²
        return 0.5 * i_mass * i_spd.norm2();
       }

    [[nodiscard]] Vect gravitational_force_on(const SphericalBody& other, const double G) const noexcept
       {// G · M·m / d²
        const Vect disp = displacement_from(other);
        const double d = disp.norm();
        return math::ratio(G * mass() * other.mass(), d*d*d) * disp;
       }

    [[nodiscard]] double gravitational_energy_with(const SphericalBody& other, const double G) const noexcept
       {// U = -G · mi·mj / d
        const double d = displacement_from(other).norm();
        return math::ratio(-G * mass() * other.mass(), d);
       }

    void apply_force(const Vect& f)
       {// Newton's law: F=ma
        i_acc = f/i_mass;
       }

    void evolve_speed(const double dt) noexcept
       {
        i_spd += dt * i_acc;
       }

    void evolve_position(const double dt) noexcept
       {
        i_pos += dt * i_spd;
       }

    void handle_possible_collision_with(SphericalBody& other, const double Cr =1.0) noexcept
       {
        // Cr: Coefficient of restitution of collisions (anelastic) 0÷1 (elastic)

        // The displacement vector from me to other
        const Vect displacement = other.displacement_from(*this);
        const double dist = displacement.norm();
        const double dist_coll = radius() + other.radius();
        if( dist <= dist_coll )
           {// Possible collision
            // Am I getting closer or moving away?
            const Vect n⃗ = displacement/dist; // Normal versor
            // My relative speed respect other
            const Vect v⃗r = speed() - other.speed();
            // Component of relative speed along the joining line
            const double vrₙ = dot_prod(v⃗r,n⃗);
            if( vrₙ>1E-3 )
               {// Collision detected!
                //dbg("Collision\n"dist:{:.6g} v⃗r:{} vrₙ:{:.6g}\n",dist, v⃗r, vrₙ);
                // Should backtrack time to know the exact collision moment
                // React to collision
                //const Vect v⃗rₙ = vrₙ * n⃗; // Normal component of relative speed
                //const Vect v⃗rₜ = speed() - v⃗rₙ; // Tangential component of relative speed
                // The exchanged impulse: In = m₁·m₂/(m₁+m₂) · (1+Cr) · vrₙ
                // Where Cr is the coefficient of restitution (anelastic) 0÷1 (elastic)
                // The speed variations: 𝛥v⃗₁=(In/m₁)·n⃗ 𝛥v⃗₂=(In/m₂)·-n⃗
                const double In = ((mass() * other.mass()) / (mass() + other.mass()))
                                  * (1.0+Cr) * vrₙ;
                // Finally, modify speeds
                i_spd += -(In/mass()) * n⃗;
                other.i_spd += (In/other.mass()) * n⃗;
               }
           }
       }

    void coalesce_with(SphericalBody& other) noexcept
       {
        // This should conserve the linear momentum
        const double combined_mass = mass() + other.mass();
        i_spd = ((mass() * speed()) + (other.mass() * other.speed())) / combined_mass;
        i_mass = combined_mass;
        i_radius = calc_radius_from_mass(i_mass);
       }

 private:
    double i_mass;
    double i_radius; // [<space>]
    Vect i_pos, // Position [<space>]
         i_spd, // Speed [<space>/<time>]
         i_acc; // Acceleration [<space>/<time>²]
};
