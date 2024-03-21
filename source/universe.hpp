#pragma once
//  ---------------------------------------------
//  N-body model
//  ---------------------------------------------
#include <vector>
#include "math-utilities.hpp" // math::*
#include "body.hpp" // SphericalBody

#include "Vect2D.hpp" // Vect2D
//#include "Vect3D.hpp" // Vect3D


////////////////////////////////////////////////////////////////////////
class Universe final
{
 public:
    const double G; // Gravitational constant. Our universe: 6.67408E-11 m³/kg s²
    using Vect = Vect2D;
    using Body = SphericalBody<Vect>;

 private:
    std::vector<Body> m_bodies;
    double t = 0.0; // [time] Elapsed time

 public:
    Universe(const double g) noexcept
      : G(g)
       {}


    //------------------------------------------------------------------------
    void evolve_euler(const double dt) noexcept
       {
        for( auto ibody=m_bodies.begin(); ibody!=m_bodies.end(); ++ibody )
           {
            const Vect f = gravitational_force_on_body(ibody);
            ibody->apply_force(f);
            ibody->evolve_speed(dt);
            ibody->evolve_position(dt);
           }
        t += dt;
       }

    //------------------------------------------------------------------------
    void evolve_verlet(const double dt) noexcept
       {
        for( Body& body : m_bodies )
           {
            body.evolve_speed(dt/2);
            body.evolve_position(dt);
           }

        for( auto ibody=m_bodies.begin(); ibody!=m_bodies.end(); ++ibody )
           {
            const Vect f = gravitational_force_on_body(ibody);
            ibody->apply_force(f);
            ibody->evolve_speed(dt/2);
           }

        t += dt;
       }

    //------------------------------------------------------------------------
    void handle_collisions() noexcept
       {
        // Normal collisions:
        //for( auto ibody=m_bodies.begin(); ibody!=m_bodies.end(); ++ibody )
        //   {
        //    for( auto iother=ibody+1; iother!=m_bodies.end(); ++iother )
        //       {
        //
        //        ibody->handle_possible_collision_with(*iother);
        //       }
        //   }

        // Coalescing colliding bodies
        // (this solves tricky problems like the proper collision detection,
        //  time backtracking, resting position)
        for( auto ibody=m_bodies.begin(); ibody!=m_bodies.end(); ++ibody )
           {
            for( auto iother=ibody+1; iother!=m_bodies.end();  )
               {
                if( iother->displacement_from(*ibody).norm2() <= math::square(ibody->radius() + iother->radius()) )
                   {// Collision detected!
                    ibody->coalesce_with(*iother);
                    // Now dispose the coalesced body
                    // Note: This won't invalidate ibody iterator, because precedes iother
                    assert(ibody<iother);
                    iother = m_bodies.erase(iother);
                   }
                else
                   {
                    ++iother;
                   }
               }
           }
       }

    [[nodiscard]] double time() const noexcept { return t; }

    [[nodiscard]] double kinetic_energy() const noexcept
       {// K = ∑ ½ m·V²
        double Ek = 0.0;
        for( const Body& body : m_bodies ) Ek += body.kinetic_energy();
        return Ek;
       }

    [[nodiscard]] double gravitational_energy() const noexcept
       {// U = ½ ∑ -G · mi·mj / d
        double Eu = 0.0;
        for( auto ibody=m_bodies.begin(); ibody!=m_bodies.end(); ++ibody )
           {// Iterate over all the other bodies
            for( auto iother=m_bodies.begin(); iother!=ibody; ++iother )
                Eu += ibody->gravitational_energy_with(*iother,G);
            for( auto iother=ibody+1; iother!=m_bodies.end(); ++iother )
                Eu += ibody->gravitational_energy_with(*iother,G);
           }
        return 0.5 * Eu;
       }

    [[nodiscard]] double total_energy() const noexcept { return kinetic_energy() + gravitational_energy(); }

    [[nodiscard]] Vect center_of_mass() const noexcept
       {// U = ∑ m·vpos / M
        Vect c;
        double total_mass = 0.0;
        for( const Body& body : m_bodies )
           {
            c += body.mass() * body.position();
            total_mass += body.mass();
           }
        return c / total_mass;
       }

    [[nodiscard]] Vect gravitational_force_on_body( std::vector<Body>::const_iterator ibody ) const noexcept
       {// F = ∑ G · m·mi / di²
        Vect f;
        // Iterate over all the other bodies
        for( auto iother=m_bodies.begin(); iother!=ibody; ++iother )
            f += iother->gravitational_force_on(*ibody,G);
        for( auto iother=ibody+1; iother!=m_bodies.end(); ++iother )
            f += iother->gravitational_force_on(*ibody,G);
        return f;
       }

    [[maybe_unused]] Universe& add_body(const double m, const Vect& pos, const Vect& spd)
       {
        m_bodies.emplace_back(m,pos,spd);
        return *this;
       }

    [[nodiscard]] const std::vector<Body>& bodies() const noexcept { return m_bodies; }
};
