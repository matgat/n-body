#ifndef GUARD_body_hpp
#define GUARD_body_hpp
//  ---------------------------------------------
//  Gravitational body descriptor
//  ---------------------------------------------
#include <stdexcept>
#include "math-utilities.hpp" // math::*


////////////////////////////////////////////////////////////////////////
template<class Vect> class Body final
{
 public:
    Body(const double m, const Vect& pos, const Vect& spd)
      : i_mass(m)
      , i_pos(pos)
      , i_spd(spd)
       {
        if( math::is_zero(i_mass) )
           {
            throw std::runtime_error("Mass cannot be null");
           }
       }

    [[nodiscard]] double mass() const noexcept { return i_mass; }
    [[nodiscard]] const Vect& position() const noexcept { return i_pos; }
    [[nodiscard]] const Vect& speed() const noexcept { return i_spd; }
    [[nodiscard]] const Vect& acceleration() const noexcept { return i_acc; }

    Vect distance_from(const Body& other) const noexcept
       {
        return position() - other.position();
       }

    [[nodiscard]] double kinetic_energy() const noexcept
       {// ½ m·V²
        return 0.5 * i_mass * i_spd.norm2();
       }

    [[nodiscard]] Vect gravitational_force_on(const Body& other, const double G) const noexcept
       {// G · M·m / d²
        const Vect disp = distance_from(other);
        const double d = disp.norm();
        return math::ratio(G * mass() * other.mass(), d*d*d) * disp;
       }

    [[nodiscard]] double gravitational_energy_with(const Body& other, const double G) const noexcept
       {// U = -G · mi·mj / d
        const double d = distance_from(other).norm();
        return math::ratio(-G * mass() * other.mass(), d);
       }

    void apply_force(const Vect& f)
       {// Newton's law: F=ma
        i_acc = (1.0/i_mass) * f;
       }

    void evolve_speed(const double dt) noexcept
       {
        i_spd += dt * i_acc;
       }

    void evolve_position(const double dt) noexcept
       {
        i_pos += dt * i_spd;
       }

 private:
    double i_mass;
    Vect i_pos, // Position [<space>]
         i_spd, // Speed [<space>/<time>]
         i_acc; // Acceleration [<space>/<time>²]
};

//---- end unit --------------------------------------------------------------
#endif
