//////////////////////////////////////////////////////////////////////////////////
//
//  General package
//
// -------------------------------------------------------------------------------
//
//  AUTHOR: Miguel Ramos Pernas
//  e-mail: miguel.ramos.pernas@cern.ch
//
//  Last update: 14/02/2017
//
// -------------------------------------------------------------------------------
//
//  Description:
//
//  Definition of instructions for the pre-processor, to be used for those classes
//  working with value-type calls.
//
// -------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////


#ifndef VALUE_TYPE_DEF
#define VALUE_TYPE_DEF


//_______________________________________________________________________________
// Performa an action in case < tp > is satisfied
#define I_ACTION_IN_CASE( tp, action ) case tp: action; break;

//_______________________________________________________________________________
// Do a static cast over the given pointer for given type
#define I_CAST_PTR( type, ptr ) ( static_cast<type*>(ptr) )

//_______________________________________________________________________________
// Delete a pointer casting it with another type
#define I_DELETE_PTR( type, ptr ) delete I_CAST_PTR(type, ptr)

//_______________________________________________________________________________
// Set an external variables < value > to the value casted for a pointer
#define I_GET_PTR_VALUE( type, ptr ) value = *I_CAST_PTR(type, ptr)

//_______________________________________________________________________________
// Create and asign a new object of a given type to a pointer
#define I_NEW_INSTANCE( type, ptr ) ptr = new type

//_______________________________________________________________________________
// Set the value attached to a pointer for a given type
#define I_SET_PTR_VALUE( type, ptr ) *I_CAST_PTR(type, ptr) = value

//_______________________________________________________________________________
// Do a switch as a function of the given data type < tp >
#define I_SWITCH_BY_DATA_TYPE( tp, ptr, action, default_act )	\
  switch( tp ) {						\
    I_ACTION_IN_CASE('B', action(char, ptr));			\
    I_ACTION_IN_CASE('b', action(unsigned char, ptr));		\
    I_ACTION_IN_CASE('S', action(short int, ptr));		\
    I_ACTION_IN_CASE('s', action(unsigned short int, ptr));	\
    I_ACTION_IN_CASE('I', action(int, ptr));			\
    I_ACTION_IN_CASE('i', action(unsigned int, ptr));		\
    I_ACTION_IN_CASE('F', action(float, ptr));			\
    I_ACTION_IN_CASE('D', action(double, ptr));			\
    I_ACTION_IN_CASE('L', action(long long int, ptr));		\
    I_ACTION_IN_CASE('l', action(unsigned long long int, ptr)); \
    I_ACTION_IN_CASE('O', action(bool, ptr));			\
  default:							\
    default_act;						\
    break;							\
  }

//_______________________________________________________________________________
// Convert the value to a string. The function std::to_string must be available for
// the given type.
#define I_TO_STRING_PTR( type, ptr ) return std::to_string( *I_CAST_PTR(type, ptr) )

#endif
