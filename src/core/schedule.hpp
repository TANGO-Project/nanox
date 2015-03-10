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

#ifndef _NANOS_SCHEDULE
#define _NANOS_SCHEDULE

#include <stddef.h>
#include <string>

#include "schedule_decl.hpp"
#include "workdescriptor_decl.hpp"
#include "atomic.hpp"
#include "functors.hpp"
#include <algorithm>
#include "synchronizedcondition_fwd.hpp"
#include "system_fwd.hpp"
#include "basethread.hpp"

using namespace nanos;

inline bool Scheduler::checkBasicConstraints ( WD &wd, BaseThread const &thread )
{
   unsigned int this_node = thread.runningOn()->getMemorySpaceId() != 0 ? sys.getSeparateMemory( thread.runningOn()->getMemorySpaceId() ).getNodeNumber() : 0;
   unsigned int tied_node = wd.isTiedLocation() ? ( wd.isTiedToLocation() != 0 ? sys.getSeparateMemory( wd.isTiedToLocation() ).getNodeNumber() : 0 ) : (unsigned int) -1; 
   bool result = wd.canRunIn(*thread.runningOn()) &&
      ( !wd.isTied() || wd.isTiedTo() == &thread ) &&
      ( !wd.isTiedLocation() || tied_node == this_node ) &&
      wd.tryAcquireCommutativeAccesses();
   //if ( thread.getId() > 0 ) *thread._file << "checkBasicConstraints says " << result << " this_node " << this_node << " tied_node " << tied_node << " isTiedToLocation " << wd.isTiedToLocation()<< std::endl;
   return result;
}

inline void SchedulerConf::setSchedulerEnabled ( const bool value )
{
   _schedulerEnabled = value;
}

inline bool SchedulerConf::getSchedulerEnabled ( void ) const
{
   return _schedulerEnabled;
}

inline unsigned int SchedulerConf::getNumSpins ( void ) const
{
   return _numSpins;
}

inline unsigned int SchedulerConf::getNumChecks ( void ) const
{
   return _numChecks;
}

inline const std::string & SchedulePolicy::getName () const
{
   return _name;
}

inline WD * SchedulePolicy::atBeforeExit  ( BaseThread *thread, WD &current, bool schedule )
{
   return 0;
}

inline WD * SchedulePolicy::atAfterExit   ( BaseThread *thread, WD *current )
{
   return atIdle( thread );
}

inline WD * SchedulePolicy::atBlock       ( BaseThread *thread, WD *current )
{
   return atIdle( thread );
}

inline WD * SchedulePolicy::atYield       ( BaseThread *thread, WD *current)
{
   return atIdle( thread );
}

inline void SchedulePolicy::atCreate ( DependableObject &depObj )
{
   return;
}

inline WD * SchedulePolicy::atWakeUp      ( BaseThread *thread, WD &wd )
{
   // Ticket #716: execute earlier tasks that have been waiting for children
   // If the WD was waiting for something
   if ( wd.started() ) {
      BaseThread * prefetchThread = NULL;
      // Check constraints since they won't be checked in Schedule::wakeUp
      if ( Scheduler::checkBasicConstraints ( wd, *thread ) ) {
         prefetchThread = thread;
      }
      else
         prefetchThread = wd.isTiedTo();
      
      // Returning the wd here makes the application to hang
      // Use prefetching instead.
      if ( prefetchThread != NULL ) {
         prefetchThread->addNextWD( &wd );
         
         return NULL;
      }
   }
   
   // otherwise, as usual
   queue( thread, wd );
   
   return NULL;
}

inline WD * SchedulePolicy::atPrefetch    ( BaseThread *thread, WD &current )
{
   return atIdle( thread );
}

inline void SchedulePolicy::atSupport    ( BaseThread *thread )
{
   return;
}

inline void SchedulePolicy::atShutdown   ( void )
{
   return;
}

inline void SchedulePolicy::atSuccessor  ( DependableObject &depObj, DependableObject &pred )
{
   return;
}

inline void SchedulePolicy::queue ( BaseThread ** threads, WD ** wds, size_t numElems )
{
   for( size_t i = 0; i < numElems; ++i )
   {
      queue( threads[i], *wds[i] );
   }
}

inline int SchedulePolicy::getPotentiallyParallelWDs ( void )
{
   return sys.getReadyNum();
}

inline void SchedulePolicySuccessorFunctor::operator() ( DependableObject *predecessor, DependableObject *successor )
{
   _obj.successorFound( predecessor, successor );
}

#endif

