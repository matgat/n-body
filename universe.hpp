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
    using Body = Body_t<Vect>;

    Universe(const double g, const double cr) noexcept
      : G(g)
      , Cr(cr) {}

    //------------------------------------------------------------------------
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

    //------------------------------------------------------------------------
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

    //------------------------------------------------------------------------
    void handle_collisions() noexcept
       {
        for( auto ibody=i_bodies.begin(); ibody!=i_bodies.end(); ++ibody )
           {
            for( auto iother=ibody+1; iother!=i_bodies.end(); ++iother )
               {
                // The displacement vector from me to other
                const Vect displacement = iother->displacement_from(*ibody);
                const double dist = displacement.norm();
                const double dist_coll = ibody->radius() + iother->radius();
                if( dist <= dist_coll )
                   {// Possible collision
                    // Am I getting closer or moving away?
                    const Vect n⃗ = displacement/dist; // Normal versor
                    // My relative speed respect other
                    const Vect v⃗r = ibody->speed() - iother->speed();
                    // Component of relative speed along the joining line
                    const double vrₙ = dot_prod(v⃗r,n⃗);
                    if( vrₙ>1E-3 )
                       {// Collision detected!
                        fmt::print(fmt::emphasis::bold | fg(fmt::color::red), "Collision\n");
                        fmt::print("dist:{:.6g} v⃗r:{} vrₙ:{:.6g}\n",dist, v⃗r, vrₙ); std::fflush(nullptr);
                        // Should backtrack time to know the exact collision moment
                        // React to collision
                        //const Vect v⃗rₙ = vrₙ * n⃗;
                        //const Vect v⃗rₜ = ibody->speed() - v⃗rₙ;
                        // The exchanged impulse: In = m₁·m₂/(m₁+m₂) · (1+Cr) · vrₙ
                        // The speed variations: 𝛥v⃗₁=(In/m₁)·n⃗ 𝛥v⃗₂=(In/m₂)·-n⃗
                        // Where Cr is the coefficient of restitution (anelastic) 0÷1 (elastic)
                        const double In = ((ibody->mass() * iother->mass()) / (ibody->mass() + iother->mass()))
                                          * (1.0+Cr) * vrₙ;

                        // Finally, modify speeds
                        ibody->set_speed(ibody->speed() + -(In/ibody->mass()) * n⃗);
                        iother->set_speed(iother->speed() + (In/iother->mass()) * n⃗);
                       }
                   }
               }
           }
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
        return c / total_mass;
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

    [[maybe_unused]] Universe& add_body(const double m, const Vect& pos, const Vect& spd)
       {
        i_bodies.emplace_back(m,pos,spd);
        return *this;
       }

    [[nodiscard]] const std::vector<Body>& bodies() const noexcept { return i_bodies; }

    const double G; // Gravitational constant. Our universe: 6.67408E-11 m³/kg s²
    const double Cr; // Coefficient of restitution of collisions (anelastic) 0÷1 (elastic)

 private:
    std::vector<Body> i_bodies;
    double t = 0.0; // [time] Elapsed time
};


//---- end unit --------------------------------------------------------------
#endif
