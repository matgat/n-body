#ifndef GUARD_universe_hpp
#define GUARD_universe_hpp
//  ---------------------------------------------
//  N-body model
//  ---------------------------------------------
#include <vector>
#include "body.hpp" // Body
#include "Vect2D.hpp" // Vect2D
//#include "Vect3D.hpp" // Vect3D


////////////////////////////////////////////////////////////////////////
class Universe final
{
 public:
    using Vect = Vect2D;
    using Body = Body<Vect>;

    Universe(const double g) noexcept : G(g), t(0.0)
       {
        i_bodies.reserve(8);
       }

    void evolve_euler(const double dt) noexcept
       {
        for( auto ibody=i_bodies.begin(); ibody!=i_bodies.end(); ++ibody )
           {
            const Vect f = gravitational_force_on_body(ibody);
            ibody->apply_force(f);
            ibody->evolve_speed(dt);
            ibody->evolve_position(dt);
           }
        t += dt;
       }

    void evolve_verlet(const double dt) noexcept
       {
        for( Body& body : i_bodies )
           {
            body.evolve_speed(dt/2);
            body.evolve_position(dt);
           }

        for( auto ibody=i_bodies.begin(); ibody!=i_bodies.end(); ++ibody )
           {
            const Vect f = gravitational_force_on_body(ibody);
            ibody->apply_force(f);
            ibody->evolve_speed(dt/2);
           }
        t += dt;
       }

    [[nodiscard]] double time() const noexcept { return t; }

    [[nodiscard]] double kinetic_energy() const noexcept
       {// K = ∑ ½ m·V²
        double Ek = 0.0;
        for( const Body& body : i_bodies ) Ek += body.kinetic_energy();
        return Ek;
       }

    [[nodiscard]] double gravitational_energy() const noexcept
       {// U = ½ ∑ -G · mi·mj / d
        double Eu = 0.0;
        for( auto ibody=i_bodies.begin(); ibody!=i_bodies.end(); ++ibody )
           {// Iterate over all the other bodies
            for( auto iother=i_bodies.begin(); iother!=ibody; ++iother )
                Eu += ibody->gravitational_energy_with(*iother,G);
            for( auto iother=ibody+1; iother!=i_bodies.end(); ++iother )
                Eu += ibody->gravitational_energy_with(*iother,G);
           }
        return 0.5 * Eu;
       }

    [[nodiscard]] double total_energy() const noexcept { return kinetic_energy() + gravitational_energy(); }

    [[nodiscard]] Vect center_of_mass() const noexcept
       {// U = ∑ m·vpos / M
        Vect c;
        double total_mass = 0.0;
        for( const Body& body : i_bodies )
           {
            c += body.mass() * body.position();
            total_mass += body.mass();
           }
        return (1.0/total_mass) * c;
       }

    [[nodiscard]] Vect gravitational_force_on_body( std::vector<Body>::const_iterator ibody ) const noexcept
       {// F = ∑ G · m·mi / di²
        Vect f;
        // Iterate over all the other bodies
        for( auto iother=i_bodies.begin(); iother!=ibody; ++iother )
            f += iother->gravitational_force_on(*ibody,G);
        for( auto iother=ibody+1; iother!=i_bodies.end(); ++iother )
            f += iother->gravitational_force_on(*ibody,G);
        return f;
       }

    const auto& add_body(const double m, const Vect& pos, const Vect& spd)
       {
        return i_bodies.emplace_back(m,pos,spd);
       }

    [[nodiscard]] const std::vector<Body>& bodies() const noexcept { return i_bodies; }

    const double G; // Gravitational constant

 private:
    std::vector<Body> i_bodies;
    double t; // [time] Elapsed time
};


//---- end unit --------------------------------------------------------------
#endif
