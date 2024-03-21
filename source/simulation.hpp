#pragma once
//  ---------------------------------------------
//  Gather data of an evolution of a N-body model
//  ---------------------------------------------
#include <fstream>
#include <string>
#include <vector>

#include "universe.hpp" // Universe


////////////////////////////////////////////////////////////////////////
class SimulationData final
{
    struct Sample final
       {
        double time;
        std::vector<Universe::Body> bodies; // Bodies status
        Universe::Vect Cm; // Center of mass
        double Ek, // Kinetic energy
               Eu; // Potential energy
       };

 private:
    std::vector<Sample> m_samples;

 public:
    [[nodiscard]] const std::vector<Sample>& samples() const noexcept { return m_samples; }

    void init(const std::size_t N) noexcept
       {
        m_samples.clear();
        m_samples.reserve(N);
       }

    void collect(const double t,
                 const std::vector<Universe::Body>& bd,
                 const Universe::Vect& cm,
                 const double ek,
                 const double eu)
       {
        m_samples.emplace_back( {t,bd,cm,ek,eu} );
       }

    void save_to_file(std::string fpath) const
       {
        std::ofstream f (fpath, std::ios::out);
        if(!f.is_open()) throw std::runtime_error("Unable to open file " + fpath);
        //f << "time,total-energy\n";
        //for(const Sample& s : m_samples)
        //    f << s.time << ',' << (s.Ek+s.Eu) << '\n';
        f << "time,bodies-xyz-pos\n";
        for(const Sample& s : m_samples)
           {
            f << s.time;
            for(const auto& body : s.bodies)
                f << ',' << body.position();
            f << '\n';
           }
       }
};


////////////////////////////////////////////////////////////////////////
class Simulation final
{
 private:
    SimulationData m_data;

 public:
    [[nodiscard]] const SimulationData& data() const noexcept { return m_data; }

    //                                 Total time    Time increment
    void execute(Universe& universe, const double T, const double dt)
       {
        m_data.init( static_cast<std::size_t>(std::ceil(T/dt)) );

        double t = 0.0;
        do {
            universe.evolve(dt);

            m_data.collect(t, universe.bodies(),
                              universe.center_of_mass(),
                              universe.kinetic_energy(),
                              universe.gravitational_energy() );
            t += dt;
           }
        while(t<T);
       }
};



//void simulate(Universe& universe)
//{
//    // Simulation settings
//    Simulation sim;
//    const double T = 100; // Total time
//    const double dt = 0.1; // Iteration time
//    fmt::print("Simulating T={} dt={}...", T, dt);
//    sim.execute(universe, T, dt);
//    fmt::print(" Done\n");
//
//    // Now do something with the simulation data...
//    // ...Edit as desired
//    //sim.data().save_to_file("data.txt");
//
//    {// Energies
//     std::string fname = "energies.txt";
//     std::ofstream f (fname, std::ios::out);
//     if(!f.is_open()) throw std::runtime_error("Unable to open file " + fname);
//     f << "time,kinetic,potential,total\n";
//     for(const Simulation::Data::Sample& s : sim.data().samples())
//         f << s.time << ',' << s.Ek << ',' << s.Eu << ',' << (s.Ek+s.Eu) << '\n';
//    }
//
//    // Bodies position
//    for( std::ptrdiff_t idx=0; [[maybe_unused]] const auto& body : universe.bodies() )
//       {
//        const std::string fname = fmt::format("body{}.txt",idx);
//        std::ofstream f (fname, std::ios::out);
//        if(!f.is_open()) throw std::runtime_error("Unable to open file " + fname);
//        f << "time,body" << idx << ".x,body" << idx << ".y,body" << idx << ".z\n";
//        for(const Simulation::Data::Sample& s : sim.data().samples())
//            f << s.time << ',' << (s.bodies.begin()+idx)->position() << '\n';
//        ++idx;
//       }
//}
