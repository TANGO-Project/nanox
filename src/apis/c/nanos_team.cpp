/*************************************************************************************/
/*      Copyright 2015 Barcelona Supercomputing Center                               */
/*                                                                                   */
/*      This file is part of the NANOS++ library.                                    */
/*                                                                                   */
/*      NANOS++ is free software: you can redistribute it and/or modify              */
/*      it under the terms of the GNU Lesser General Public License as published by  */
/*      the Free Software Foundation, either version 3 of the License, or            */
/*      (at your option) any later version.                                          */
/*                                                                                   */
/*      NANOS++ is distributed in the hope that it will be useful,                   */
/*      but WITHOUT ANY WARRANTY; without even the implied warranty of               */
/*      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                */
/*      GNU Lesser General Public License for more details.                          */
/*                                                                                   */
/*      You should have received a copy of the GNU Lesser General Public License     */
/*      along with NANOS++.  If not, see <http://www.gnu.org/licenses/>.             */
/*************************************************************************************/

/*! \file nanos_team.cpp
 *  \brief 
 */
#include "nanos.h"
#include "system.hpp"
#include "basethread.hpp"
#include "debug.hpp"
#include "instrumentationmodule_decl.hpp"
#include "instrumentation_decl.hpp"

using namespace nanos;

/*! \defgroup capi_team Thread team services.
 *  \ingroup capi
 */
/*! \addtogroup capi_team
 *  \{
 */

/*! \brief Creates a new team
 *  
 *  \param team Resulting team
 *  \param sp Scheduling policy
 *  \param nthreads Number of threads
 *  \param constraints List of constraints
 *  \param reuse Reuse current thread for the new team
 *  \param info Extra information needed by team
 *  \sa ThreadTeam
 */
NANOS_API_DEF(nanos_err_t, nanos_create_team, ( nanos_team_t *team, nanos_sched_t sp, unsigned int *nthreads,
                               nanos_constraint_t * constraints, bool reuse, nanos_thread_t *info, nanos_const_wd_definition_t *const_data_ext ))
{
   unsigned i = 0;

   NANOS_INSTRUMENT( nanos_const_wd_definition_internal_t *const_data = reinterpret_cast<nanos_const_wd_definition_internal_t*>(const_data_ext); )

   NANOS_INSTRUMENT( static Instrumentation *INS = sys.getInstrumentation(); )
   NANOS_INSTRUMENT( static InstrumentationDictionary *ID = INS->getInstrumentationDictionary(); )
   NANOS_INSTRUMENT( static nanos_event_key_t api_key = ID->getEventKey("api"); )
   NANOS_INSTRUMENT( static nanos_event_value_t api_value = ID->getEventValue("api","create_team"); )
   NANOS_INSTRUMENT( static nanos_event_key_t threads_key = ID->getEventKey("set-num-threads"); )
   NANOS_INSTRUMENT( static nanos_event_key_t parallel_ol_key = ID->getEventKey("parallel-outline-fct"); )

   NANOS_INSTRUMENT( Instrumentation::Event events[4]; )
   NANOS_INSTRUMENT( INS->createStateEvent( &events[i++], NANOS_RUNTIME ); )
   NANOS_INSTRUMENT( INS->createBurstEvent( &events[i++], api_key, api_value ); )
   NANOS_INSTRUMENT( INS->createPointEvent( &events[i++], threads_key, (nanos_event_value_t ) *nthreads ); )
   NANOS_INSTRUMENT ( if ( const_data != NULL ) { )
   NANOS_INSTRUMENT( INS->createPointEvent( &events[i++], parallel_ol_key, (nanos_event_value_t ) ((nanos_smp_args_t *)(const_data->devices[0].arg))->outline ); )
   NANOS_INSTRUMENT ( } else { )
   NANOS_INSTRUMENT( INS->createPointEvent( &events[i++], parallel_ol_key, (nanos_event_value_t ) NULL ) ; )
   NANOS_INSTRUMENT ( } )
   NANOS_INSTRUMENT( INS->addEventList ( i, events ); )

   try {
      if ( *team ) warning( "pre-allocated team not supported yet" );
      if ( sp ) warning ( "selecting scheduling policy not supported yet");

      //! \note Calling system create team with number of threads, constraints, reuse master and
      // not entering in the team at creation
      ThreadTeam *new_team = sys.createTeam( *nthreads, constraints, reuse, false, true );

      *team = new_team;

      *nthreads = new_team->size();

      for ( i = 0; i < new_team->size(); i++ )
         info[i] = ( nanos_thread_t ) &( *new_team )[i];
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_create_team_mapped, ( nanos_team_t *team, nanos_sched_t sg, unsigned int *nthreads,
                                       unsigned int *mapping ))
{
   return NANOS_UNIMPLEMENTED;
}

NANOS_API_DEF(nanos_err_t, nanos_enter_team, (void))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","enter_team",NANOS_RUNTIME) );
   try {
      myThread->lock();
      myThread->enterTeam( NULL );
      myThread->unlock();
   } catch ( nanos_err_t e) {
      return e;
   }
   return NANOS_OK;

}
NANOS_API_DEF(nanos_err_t, nanos_leave_team, (void))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","leave_team",NANOS_RUNTIME) );

   try {
      ThreadTeam* t = myThread->getTeam();
      myThread->leaveTeam( );
      t->decreaseFinalSize();
   } catch ( nanos_err_t e) {
      return e;
   }
   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_end_team, ( nanos_team_t team ))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","end_team",NANOS_RUNTIME) );

   try {
      sys.endTeam((ThreadTeam *)team);
   } catch ( nanos_err_t e) {
         return e;
   }
   return NANOS_OK;
}

/*!
   Implements the team barrier by invoking the barrier function of the team.
   The actual barrier algorithm is loaded at the run-time startup.
*/
NANOS_API_DEF(nanos_err_t, nanos_team_barrier, ( void ))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","team_barrier",NANOS_SYNCHRONIZATION) );

   try {
      myThread->getTeam()->barrier();
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_team_get_num_starring_threads_FIXME, ( int *n ))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_num_starring_threads",NANOS_RUNTIME) );

   try {
      *n = myThread->getTeam()->getNumStarringThreads();
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_team_get_starring_threads_FIXME, ( int *n, nanos_thread_t *list_of_threads ) )
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_starring_threads",NANOS_RUNTIME) );

   try {
      *n = myThread->getTeam()->getStarringThreads( (BaseThread **) list_of_threads );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_team_get_num_supporting_threads, ( int *n ))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_num_supporting_threads",NANOS_RUNTIME) );

   try {
      *n = myThread->getTeam()->getNumSupportingThreads();
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_team_get_supporting_threads, ( int *n, nanos_thread_t *list_of_threads))
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_supporting_threads",NANOS_RUNTIME) );

   try {
      *n = myThread->getTeam()->getSupportingThreads( (BaseThread **) list_of_threads );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_register_reduction, ( nanos_reduction_t *red) )
{
   //NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_supporting_threads",NANOS_RUNTIME) );

   try {
       myThread->getTeam()->createReduction ( red );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_reduction_get_private_data, ( void **copy, void *original ) )
{
   //NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","get_supporting_threads",NANOS_RUNTIME) );

   try {
       *copy = (void *) myThread->getTeam()->getReductionPrivateData ( original );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_reduction_get, (nanos_reduction_t** dest, void *original) )
{

   try {
       *dest = myThread->getTeam()->getReduction ( original );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_admit_current_thread, (void))
{

   try {
       sys.admitCurrentThread( true );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t, nanos_expel_current_thread, (void))
{

   try {
       sys.expelCurrentThread( true );
   } catch ( nanos_err_t e) {
      return e;
   }

   return NANOS_OK;
}
NANOS_API_DEF (nanos_err_t, nanos_task_reduction_register, ( void *orig, void *dep, size_t size, size_t align, void (*init)( void *, void * ), void (*reducer)( void *, void * ), void (*reducer_orig_var)( void *, void * ) ) )
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","task_reduction_register",NANOS_RUNTIME) );
   try {
#ifndef ON_TASK_REDUCTION
       myThread->getTeam()->registerTaskReduction ( orig, dep, size, init, reducer, reducer_orig_var);
#else
       myThread->getCurrentWD()->registerTaskReduction ( orig, dep, size, init, reducer, reducer_orig_var);
#endif
   } catch ( nanos_err_t e) {
      return e;
   }
   return NANOS_OK;
}

NANOS_API_DEF (nanos_err_t, nanos_task_reduction_get_thread_storage, ( void *orig, void **tpd ) )
{
   NANOS_INSTRUMENT( InstrumentStateAndBurst inst("api","task_reduction_get_thread_storage",NANOS_RUNTIME) );
   try {
#ifndef ON_TASK_REDUCTION
      *tpd = myThread->getTeam()->getTaskReductionThreadStorage ( orig, myThread->getTeamId() );
#else
      *tpd = myThread->getCurrentWD()->getTaskReductionThreadStorage ( orig, myThread->getTeamId() );
#endif
   } catch ( nanos_err_t e) {
      return e;
   }
   return NANOS_OK;
}
/*!
 * \}
 */ 
