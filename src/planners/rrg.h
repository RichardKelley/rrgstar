/*! \file planners/rrg.h
  \brief An implementation of a Rapidly-exploring Random Graph (RRG) algorithm.

  Provides an implementation of the RRG algorithm. Inherits from the generic
  incremental sampling-based motion planner, overriding the iteration function.
 */

#ifndef _RRGLIB_PLANNER_RRG_H_
#define _RRGLIB_PLANNER_RRG_H_


#include <components/model_checkers/mu_calculus.h>
#include <planners/base_incremental.h>
#include <planners/planner_parameters.h>


namespace rrglib {

    //! RRG algorithm
    /*!
      Provides an implementation of the RRG algorithm. Inherits from the generic
      incremental sampling-based motion planner, overriding the iteration function.

      \ingroup planners
    */
    template< class typeparams >
    class rrg : public planner_incremental< typeparams > {


        typedef typename typeparams::state state_t;
        typedef typename typeparams::input input_t;

        typedef trajectory<typeparams> trajectory_t;

        typedef vertex<typeparams> vertex_t;
        typedef edge<typeparams> edge_t;

        typedef planner_incremental<typeparams> planner_t;

        typedef planner_parameters parameters_t;

        typedef sampler_base<typeparams> sampler_t;
        typedef distance_evaluator_base<typeparams> distance_evaluator_t;
        typedef extender_base<typeparams> extender_t;
        typedef collision_checker_base<typeparams> collision_checker_t;
        typedef model_checker_mu_calculus<typeparams> model_checker_t;

    private:

        model_checker_t &model_checker;


        // This function adds the given state to the beginning of the trajectory and calls the collision checker.
        int check_extended_trajectory_for_collision (state_t *state, trajectory_t *trajectory) {

            trajectory->list_states.push_front (state);
            int collision_check = this->collision_checker.check_collision_trajectory (trajectory);
            trajectory->list_states.pop_front ();

            return collision_check;
        }



    public:

        //! Algorithm parameters
        /*!
          This class stores the parameters used by the algorithm. These parameters
          can be modified by the user using the methods provided by the class
          planner_parameters.
        */
        parameters_t parameters;


        rrg ();
        ~rrg ();

        /**
         * \brief A constructor that initializes all components.
         *
         * This is the recommended constructor that initializes all components all at once.
         * It calls the corresponding constructor of the base class
         * planner_incremental<typeparams> with the same arguments.
         *
         * @param sampler_in New sampler component.
         * @param distance_evaluator_in New distance evaluator component.
         * @param extender_in New extension function component.
         * @param collision_checker_in New collision checker component.
         * @param model_checker_in New model checker component.
         */
        rrg (sampler_t &sampler_in, distance_evaluator_t &distance_evaluator_in, extender_t &extender_in,
             collision_checker_t &collision_checker_in, model_checker_t &model_checker_in);


        /**
         * \brief A function call to initiate one iteration of the algorithm.
         *
         * Runs one iteration of the RRG algorithm which includes the following steps:
         * - get one sample state (using the sampler component)
         * - find the vertex in the graph that is nearest to the sample state
         * (using the distance evaluator component)
         * - generate a trajectory that starts from the state stored in the nearest
         * vertex and reaches exactly or approximately to the sample state (using
         * the extension function component)
         * - check whether the new trajectory satsifies the conditions for being
         * collision free (using the collision checker component).
         * - if the new trajectory is collision free, then
         *   - add new trajectory to the graph as an edge and create a new vertex, called
         *     the extended vertex that connects to the end of the new trajectory.
         *   - incrementally check whether the graph includes a trajectory that
         *     satisfies the termination requirement (using the model checker component).
         *   - compute the set of near vertices (using the distance evaluator component).
         *   - for all vertecies in the near set
         *     - generate a new trajectory from the near vertex to the extended vertex
         *       (using the extension function component).
         *     - if the new trajectory is collision free (check using the collision checker
         *       component) and exactly connects the two vertices,
         *       then add the new trajectory to the graph as an edge from the near vertex to
         *       the extended vertex.
         *     - incrementally check whether the graph includes a trajectory that
         *       satisfies the termination requirement (using the model checker component).
         *   - for all vertices in the near set
         *     - generate a new trajectory from the extended vertex to the near vertex
         *       (using the extension function component).
         *     - if the new trajectory is collision free (check using the collision checker
         *       component) and exactly connects the two vertices,
         *       then add the new trajectory to the graph as an edge from the extended vertex to
         *       the near vertex.
         *     - incrementally check whether the graph includes a trajectory that
         *       satisfies the termination requirement (using the model checker component).
         *
         * @returns Returns 1 for success, and a non-positive number for failure.
         */
        int iteration ();

        bool has_feasible() const;

        /* Print in JSON format to output stream (default is std::cout) the
           sampled graph entirely in terms of states and a solution if found by
           the RRG planner. The intermediate states corresponding to edges are
           also included.  Specifically, the JSON object has the following keys:

           1. "has_feasible" (bool) indicating whether a feasible trajectory was found,

           2. "solution" (K x N array) states in order, defined only if has_feasible,

           3. "V", an object in which all keys are addresses (type string) of
              vertices in the RRG; the value for each is another object (a.k.a.,
              key-value or dictionary) with the following keys. "state" is an
              array that is the state at that vertex. "successors" is another
              object with keys of addresses of vertices that are adjacent via an
              outgoing edge; each has as value an array of the states in the
              trajectory that realizes the edge. */
        void dump_json( bool include_graph = true ) const;
        void dump_json( std::ostream &s, bool include_graph ) const;
    };

}


#endif
