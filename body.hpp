#ifndef GUARD_body_hpp
#define GUARD_body_hpp
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
       {// ?? m??V??
        return 0.5 * i_mass * i_spd.norm2();
       }

    [[nodiscard]] Vect gravitational_force_on(const SphericalBody& other, const double G) const noexcept
       {// G ?? M??m / d??
        const Vect disp = displacement_from(other);
        const double d = disp.norm();
        return math::ratio(G * mass() * other.mass(), d*d*d) * disp;
       }

    [[nodiscard]] double gravitational_energy_with(const SphericalBody& other, const double G) const noexcept
       {// U = -G ?? mi??mj / d
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
        // Cr: Coefficient of restitution of collisions (anelastic) 0??1 (elastic)

        // The displacement vector from me to other
        const Vect displacement = other.displacement_from(*this);
        const double dist = displacement.norm();
        const double dist_coll = radius() + other.radius();
        if( dist <= dist_coll )
           {// Possible collision
            // Am I getting closer or moving away?
            const Vect n??? = displacement/dist; // Normal versor
            // My relative speed respect other
            const Vect v???r = speed() - other.speed();
            // Component of relative speed along the joining line
            const double vr??? = dot_prod(v???r,n???);
            if( vr???>1E-3 )
               {// Collision detected!
                //fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Collision\n");
                //fmt::print("dist:{:.6g} v???r:{} vr???:{:.6g}\n",dist, v???r, vr???); std::fflush(nullptr);
                // Should backtrack time to know the exact collision moment
                // React to collision
                //const Vect v???r??? = vr??? * n???; // Normal component of relative speed
                //const Vect v???r??? = speed() - v???r???; // Tangential component of relative speed
                // The exchanged impulse: In = m?????m???/(m???+m???) ?? (1+Cr) ?? vr???
                // Where Cr is the coefficient of restitution (anelastic) 0??1 (elastic)
                // The speed variations: ????v??????=(In/m???)??n??? ????v??????=(In/m???)??-n???
                const double In = ((mass() * other.mass()) / (mass() + other.mass()))
                                  * (1.0+Cr) * vr???;
                // Finally, modify speeds
                i_spd += -(In/mass()) * n???;
                other.i_spd += (In/other.mass()) * n???;
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
         i_acc; // Acceleration [<space>/<time>??]
};

//---- end unit --------------------------------------------------------------
#endif
