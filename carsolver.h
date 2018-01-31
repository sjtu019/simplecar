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
 * File:   carsolver.h
 * Author: Jianwen Li
 * Note: An inheritance class from Minisat::Solver for CAR use 
 * Created on October 4, 2017
 */
 
#ifndef CAR_SOLVER_H
#define	CAR_SOLVER_H

//#include "minisat/core/Solver.h"
#include "glucose/core/Solver.h"
#include <vector>

namespace car
{
	class CARSolver : public Glucose::Solver 
	{
	public:
		CARSolver () {}
		CARSolver (bool verbose) : verbose_ (verbose) {} 
		bool verbose_;
		
		//Minisat::vec<Minisat::Lit> assumption_;  //Assumption for SAT solver
		Glucose::vec<Glucose::Lit> assumption_;  //Assumption for SAT solver
		
		//functions
		bool solve_assumption ();
		std::vector<int> get_model ();    //get the model from SAT solver
 		std::vector<int> get_uc ();       //get UC from SAT solver
		
		void add_cube (const std::vector<int>&);
		void add_clause_from_cube (const std::vector<int>&);
		void add_clause (int);
 		void add_clause (int, int);
 		void add_clause (int, int, int);
 		void add_clause (int, int, int, int);
 		void add_clause (std::vector<int>&);
 	
 		//Minisat::Lit SAT_lit (int id); //create the Lit used in SAT solver for the id.
 		//int lit_id (Minisat::Lit);  //return the id of SAT lit
 		
 		Glucose::Lit SAT_lit (int id); //create the Lit used in SAT solver for the id.
 		int lit_id (Glucose::Lit);  //return the id of SAT lit
 		
 		inline int size () {return clauses.size ();}
 		inline void clear_assumption () {assumption_.clear ();}
 		
 		
 		//printers
 		void print_clauses ();
 		void print_assumption ();
 		
 		//l <-> r
 		inline void add_equivalence (int l, int r)
 		{
 			add_clause (-l, r);
 			add_clause (l, -r);
 		}
 	
 		//l <-> r1 /\ r2
 		inline void add_equivalence (int l, int r1, int r2)
 		{
 			add_clause (-l, r1);
 			add_clause (-l, r2);
 			add_clause (l, -r1, -r2);
 		}
 	
 		//l<-> r1 /\ r2 /\ r3
 		inline void add_equivalence (int l, int r1, int r2, int r3)
 		{
 			add_clause (-l, r1);
 			add_clause (-l, r2);
 			add_clause (-l, r3);
 			add_clause (l, -r1, -r2, -r3);
 		}
	};
}

#endif
