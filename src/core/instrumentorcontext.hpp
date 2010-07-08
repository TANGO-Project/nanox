/*************************************************************************************/
/*      Copyright 2009 Barcelona Supercomputing Center                               */
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
#ifndef __NANOS_INSTRUMENTOR_CTX_H
#define __NANOS_INSTRUMENTOR_CTX_H
#include <stack>
#include <list>

#include "instrumentorcontext_decl.hpp"
#include "instrumentor.hpp"
#include "debug.hpp"

using namespace nanos;

#ifdef NANOS_INSTRUMENTATION_ENABLED

inline void InstrumentationContext::pushState ( nanos_event_state_value_t state )
{
   _stateStack.push( state );
}

inline void InstrumentationContext::popState ( void )
{
   if ( !(_stateStack.empty()) ) _stateStack.pop();
}

inline nanos_event_state_value_t InstrumentationContext::topState ( void )
{
   if ( !(_stateStack.empty()) ) return _stateStack.top();
   else return ERROR;
}

inline bool InstrumentationContext::findBurstByKey ( nanos_event_key_t key, InstrumentationContext::BurstIterator &ret )
{
   bool found = false;
   BurstList::iterator it;

   for ( it = _burstList.begin() ; !found && (it != _burstList.end()) ; it++ ) {
      Event::ConstKVList kvlist = (*it).getKVs();
      if ( kvlist[0].first == key  ) { ret = it; found = true;}
   }

   return found;
}

inline unsigned int InstrumentationContext::getNumBursts() const
{
   return _burstList.size();
}

inline InstrumentationContext::ConstBurstIterator InstrumentationContext::beginBurst() const
{
   return _burstList.begin();
}

inline InstrumentationContext::ConstBurstIterator InstrumentationContext::endBurst() const
{
   return _burstList.end();
}

inline void InstrumentationContext::disableStateEvents ( void )
{
   _stateEventEnabled = false;
}

inline void InstrumentationContext::enableStateEvents ( void )
{
   _stateEventEnabled = true;
}

inline bool InstrumentationContext::isStateEventEnabled ( void )
{
   return _stateEventEnabled;
}

inline nanos_event_state_value_t InstrumentationContext::getValidState ( void )
{
   return _validState;
}

inline void InstrumentationContext::setValidState ( nanos_event_state_value_t state )
{
   _validState = state;
}

#endif

#endif
