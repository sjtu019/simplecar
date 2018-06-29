/*
    Copyright (C) 2018, Jianwen Li (lijwen2748@gmail.com), Iowa State University

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
	Author: Jianwen Li
	Update Date: September 8, 2017
	Interface for the checker class
*/

 #ifndef CHECKER_H
 #define CHECKER_H
 
#include "data_structure.h"
#include "invsolver.h"
#include "startsolver.h"
#include "mainsolver.h"
#include "model.h"
#include <assert.h>
#include "utility.h"
#include "statistics.h"
#include <fstream>

#define MAX_SOLVER_CALL 500

namespace car 
{
	class Checker 
	{
	public:
		Checker (Model* model, Statistics& stats, std::ofstream* dot, bool forward = true, bool evidence = false, bool verbose = false, bool minimal_uc = false);
		~Checker ();
		
		bool check (std::ofstream&);
		void print_evidence (std::ofstream&);
		inline int frame_size () {return frame_.size ();}
		inline void print_frames_sizes () {
		    for (int i = 0; i < F_.size (); i ++) {
		        std::cout << F_[i].size () << " ";
		    }
		    std::cout << std::endl;
		}
	private:
		//flags 
		bool forward_;
		bool partial_state_;
		bool minimal_uc_;
		bool evidence_;
		bool verbose_;
		//members
		Statistics *stats_;
		
		std::ofstream* dot_; //for dot file
		int solver_call_counter_; //counter for solver_ calls
		int start_solver_call_counter_; //counter for start_solver_ calls
		
		int minimal_update_level_;
		State* init_;  // the start state for forward CAR
		State* last_;  // the start state for backward CAR
		int bad_;

		Model* model_;
		MainSolver *solver_;
		StartSolver *start_solver_;
		InvSolver *inv_solver_;
		Fsequence F_;
		Bsequence B_;
		Frame frame_;   //to store the frame willing to be added in F_ in one step
		
		bool safe_reported_;  //true means ready to return SAFE
		//functions
		bool immediate_satisfiable ();
		bool immediate_satisfiable (const State*);
		bool immediate_satisfiable (const Cube&);
		void initialize_sequences ();
		bool try_satisfy (const int frame_level);
		int do_search (const int frame_level);
		bool try_satisfy_by (int frame_level, const State* s);
		bool invariant_found (int frame_level);
		bool invariant_found_at (const int frame_level);
		void inv_solver_add_constraint_or (const int frame_level);
		void inv_solver_add_constraint_and (const int frame_level);
		void inv_solver_release_constraint_and ();
		bool solve_with (const Cube &cu, const int frame_level);
		State* get_new_state (const State *s);
		void extend_F_sequence ();
		void update_F_sequence (const State* s, const int frame_level);
		void update_frame_by_relative (const State* s, const int frame_level);
		void update_B_sequence (State* s);
		int get_new_level (const State *s, const int frame_level);
		void push_to_frame (Cube& cu, const int frame_level);
		bool tried_before (const State* s, const int frame_level);
		
		
		State* enumerate_start_state ();
		State* get_new_start_state ();
		std::pair<Assignment, Assignment> state_pair (const Assignment& st);
		
		void car_initialization ();
		void car_finalization ();
		void destroy_states ();
		bool car_check ();
				
		
		//inline functions
		inline void create_inv_solver (){
			inv_solver_ = new InvSolver (model_, verbose_);
		}
		inline void delete_inv_solver (){
			delete inv_solver_;
			inv_solver_ = NULL;
		}
		inline void report_safe (){
		    safe_reported_ = true;
		}
		inline bool safe_reported (){
		    return safe_reported_;
		}
		
		inline void reset_start_solver (){
	        assert (start_solver_ != NULL);
	        start_solver_->reset ();
	    }
	    
	    inline bool reconstruct_start_solver_required () {
	        start_solver_call_counter_ ++;
	        if (start_solver_call_counter_ == MAX_SOLVER_CALL) {
	            start_solver_call_counter_ = 0;
	            return true;
	        }
	        return false;
	    }
	    
	    inline void reconstruct_start_solver () {
	        delete start_solver_;
	        start_solver_ = new StartSolver (model_, bad_, forward_, verbose_);
	        for (int i = 0; i < frame_.size (); i ++) {
	            start_solver_->add_clause_with_flag (frame_[i]);
	        }
	    }
	    
	    inline bool start_solver_solve_with_assumption (){
	        if (reconstruct_start_solver_required ())
	            reconstruct_start_solver ();
	            
	        stats_->count_start_solver_SAT_time_start ();
	    	bool res = start_solver_->solve_with_assumption ();
	    	stats_->count_start_solver_SAT_time_end ();
	    	return res;
	    }
	    
	    inline bool reconstruct_solver_required () {
	        solver_call_counter_ ++;
	        if (solver_call_counter_ == MAX_SOLVER_CALL) {
	            solver_call_counter_ = 0;
	            return true;
	        }
	        return false;
	    }
	    
	    inline void reconstruct_solver () {
	        delete solver_;
	        MainSolver::clear_frame_flags ();
	        solver_ = new MainSolver (model_, stats_, verbose_);
	        for (int i = 0; i < F_.size (); i ++) {
	            solver_->add_new_frame (F_[i], i, forward_);
	        }
	    }
	    
	    inline bool solver_solve_with_assumption (const Assignment& st, const int p){
	        if (reconstruct_solver_required ())
	            reconstruct_solver ();
	        stats_->count_main_solver_SAT_time_start ();
	        bool res = solver_->solve_with_assumption (st, p);
	        stats_->count_main_solver_SAT_time_end ();
	        return res;
	    }
	    
	    inline bool solver_solve_with_assumption (const Assignment& st, const int frame_level, bool forward){
	        if (reconstruct_solver_required ())
	            reconstruct_solver ();
	        solver_->set_assumption (st, frame_level, forward);
	        stats_->count_main_solver_SAT_time_start ();
		    bool res = solver_->solve_with_assumption ();
		    stats_->count_main_solver_SAT_time_end ();
		    return res;
	    }
	    
	    inline void clear_frame (){
	        frame_.clear ();
	        for (int i = 0; i < frame_.size (); i ++)
	        	start_solver_->add_clause_with_flag (frame_[i]);
	    }
	    
	    
	    inline void print_frame (const Frame& f){
	        for (int i = 0; i < f.size (); i ++)
	            car::print (f[i]);
	    }
	    
	    inline void print_F (){
	        std::cout << "-----------F sequence information------------" << std::endl;
	        for (int i = 0; i < F_.size (); i ++){
	            std::cout << "Frame " << i << ":" << std::endl;
	            print_frame (F_[i]);
	        }
	        std::cout << "-----------End of F sequence information------------" << std::endl;
	    }
	    
	    inline void print_B (){
	        std::cout << "-----------B sequence information------------" << std::endl;
	        for (int i = 0; i < B_.size (); i ++){
	            for (int j = 0; j < B_[i].size (); j ++)
	                B_[i][j]->print ();
	        }
	        std::cout << "-----------End of B sequence information------------" << std::endl;
	    }
	    
	    inline void print (){
	        print_F ();
	        print_B ();
	    }
	    
	};
}
#endif
