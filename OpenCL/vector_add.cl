__kernel void ocl_vector_add(__global int *a, __global int *b)
{
	__local int results[64];  // Actually we don't need this, just for illustration
	
	int global_thread_id = get_group_id(0) * get_local_size(0) + get_local_id(0);
	int local_thread_id  = get_local_id(0);
	
	results[local_thread_id] = a[global_thread_id] + b[global_thread_id];
	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	a[global_thread_id] = results[local_thread_id];
}