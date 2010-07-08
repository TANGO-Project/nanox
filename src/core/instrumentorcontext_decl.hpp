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
#ifndef __NANOS_INSTRUMENTOR_CTX_DECL_H
#define __NANOS_INSTRUMENTOR_CTX_DECL_H
#include <stack>
#include <list>

#include "instrumentor.hpp"
#include "debug.hpp"

namespace nanos {

   class InstrumentationContext {
#ifdef NANOS_INSTRUMENTATION_ENABLED
      friend class Instrumentation;
      friend class InstrumentationContextStackedBursts;

      protected:
         typedef Instrumentation::Event Event;
         typedef Instrumentation::Burst Burst;
         typedef std::stack<nanos_event_state_value_t> StateStack;
         typedef std::list<Event> BurstList;

         StateStack                 _stateStack;           /**< Stack of states */
         bool                       _stateEventEnabled;    /**< Set state level, zero by default */
         nanos_event_state_value_t  _validState;           /**< Last valid states */
         BurstList                  _burstList;            /**< List of current opened bursts */
         BurstList                  _burstBackup;          /**< Backup list (non-active) of opened bursts */

      public:
         /*! \brief InstrumenotrContext const BurstIterator
          */
         typedef BurstList::const_iterator   ConstBurstIterator;

         /*! \brief InstrumenotrContext (non-const) BurstIterator
          */
         typedef BurstList::iterator         BurstIterator;

         /*! \brief InstrumentationContext copy constructor
          */
         explicit InstrumentationContext(const InstrumentationContext &ic) : _stateStack(), _stateEventEnabled(ic._stateEventEnabled),
                                                                             _burstList(), _burstBackup() {}

         /*! \brief InstrumentationContext copy constructor
          */
         explicit InstrumentationContext(const InstrumentationContext *ic) : _stateStack(), _stateEventEnabled(ic->_stateEventEnabled),
                                                                             _burstList(), _burstBackup() {}

         /*! \brief InstrumentationContext constructor
          */
         InstrumentationContext () :_stateStack(), _stateEventEnabled(true), _burstList(), _burstBackup() { }

         /*! \brief InstrumentationContext destructor
          */
         virtual ~InstrumentationContext() {}

       protected: /* Only friend classes (Instrumentor) can use InstrumentationContext */

         /*! \brief Adds a state value into the state stack 
          */
         void pushState ( nanos_event_state_value_t state ); 

         /*! \brief Removes top state from the state stack
          */
         void popState ( void ); 

         /*! \brief Gets current state from top of stack
          */
         nanos_event_state_value_t topState ( void );

         /*! \brief Inserts a Burst into the burst list
          *
          *  This function inserts a burst event in the burst list. If an event with the same type of that
          *  event was already in the list it will be moved to an internal backup list in order to guarantee
          *  just one event per type in the list.
          */
         virtual void insertBurst ( const Event &e );

         /*! \brief Removes a Burst from the burst list
          *
          *  This function removes a burst event from the burst list. If an event with the same type of that
          *  event was in the backup list it will be recovered to main list.
          */
         virtual void removeBurst ( BurstIterator it ); 

         /*! \brief Look for a specific event given its key value
          */
         bool findBurstByKey ( nanos_event_key_t key, BurstIterator &ret );


         /*! \brief Get the number of bursts in the main list
          */
         unsigned int getNumBursts() const ; 

         /*! \brief Gets the starting element in the burst list
          */
         ConstBurstIterator beginBurst() const ; 

         /*! \brief Gets the last element in the burst list
          */
         ConstBurstIterator endBurst() const ; 

         /*! \brief Enable state events
          */
         void enableStateEvents ( void ) ;

         /*! \brief Disable state events
          */
         void disableStateEvents ( void ) ;

         /*! \brief Get state events status
          */
         bool isStateEventEnabled ( void ) ;

         /*! \brief Get valid state
          */
         nanos_event_state_value_t getValidState ( void ) ;

         /*! \brief Save current state as valid state
          */
         void setValidState ( nanos_event_state_value_t state ) ;
#endif
   };

   class InstrumentationContextStackedBursts : public InstrumentationContext {
#ifdef NANOS_INSTRUMENTATION_ENABLED
      public:
         InstrumentationContextStackedBursts () : InstrumentationContext() {}
         InstrumentationContextStackedBursts ( const InstrumentationContext &ic) : InstrumentationContext ( ic ) {}
         InstrumentationContextStackedBursts ( InstrumentationContext *ic) : InstrumentationContext ( ic ) {}
         ~InstrumentationContextStackedBursts () {}
    
         void insertBurst ( const Event &e );
         void removeBurst ( BurstIterator it ); 
#endif
   };
}
#endif
