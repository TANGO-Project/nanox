
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

#include "nanos-opencl.h"
#include "nanos.h"
#include "system.hpp"
#include "openclprocessor.hpp"
#include "opencldd.hpp"
#include <string.h>


using namespace nanos;

NANOS_API_DEF(void *, nanos_opencl_factory, (void *args)) {
    nanos_opencl_args_t *opencl = (nanos_opencl_args_t *) args;
    return (void *) new ext::OpenCLDD(opencl->outline);
}

NANOS_API_DEF(void*, nanos_create_current_kernel, (char* kernel_name, char* opencl_code, char* compiler_opts)){  
    nanos::ext::OpenCLProcessor *pe=( nanos::ext::OpenCLProcessor * ) getMyThreadSafe()->runningOn();
    return pe->createKernel(kernel_name,opencl_code,compiler_opts);
}

NANOS_API_DEF(nanos_err_t,nanos_opencl_set_bufferarg, (void* opencl_kernel, int arg_num, void* pointer)){
   try {
      nanos::ext::OpenCLProcessor *pe=( nanos::ext::OpenCLProcessor * ) getMyThreadSafe()->runningOn();
      pe->setKernelBufferArg(opencl_kernel, arg_num, pointer);
   } catch ( ... ) {
      return NANOS_UNKNOWN_ERR;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t,nanos_opencl_set_arg, (void* opencl_kernel, int arg_num, size_t size, void* pointer)){
    try {
      nanos::ext::OpenCLProcessor *pe=( nanos::ext::OpenCLProcessor * ) getMyThreadSafe()->runningOn();
      pe->setKernelArg(opencl_kernel, arg_num, size, pointer);
   } catch ( ... ) {
      return NANOS_UNKNOWN_ERR;
   }

   return NANOS_OK;
}

NANOS_API_DEF(nanos_err_t,nanos_exec_kernel, (void* opencl_kernel, int work_dim, size_t* ndr_offset, size_t* ndr_local_size, size_t* ndr_global_size)){
    try {
      nanos::ext::OpenCLProcessor *pe=( nanos::ext::OpenCLProcessor * ) getMyThreadSafe()->runningOn();
      pe->execKernel(opencl_kernel, work_dim, ndr_offset, ndr_local_size, ndr_global_size);
   } catch ( ... ) {
      return NANOS_UNKNOWN_ERR;
   }

   return NANOS_OK;
}