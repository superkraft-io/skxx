#include "sk_soft_backend_bundle_group_<!id!>.h"

BEGIN_SK_NAMESPACE

size_t group_<!id!>_offsets_array[<!offsets_arr_size!>] = {<!offsets!>};
size_t group_<!id!>_sizes_array[<!sizes_arr_size!>] = {<!sizes!>};

#ifndef __INTELLISENSE__

const unsigned char group_<!id!>_data_array[] = {<!data!>};

#endif __INTELLISENSE__

size_t* group_<!id!>_offsets             = group_<!id!>_offsets_array;
size_t* group_<!id!>_sizes               = group_<!id!>_sizes_array;
const size_t group_<!id!>_data_size      = <!data_size!>;
const unsigned char* const group_<!id!>_data = group_<!id!>_data_array;

END_SK_NAMESPACE