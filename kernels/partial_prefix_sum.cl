__kernel void partial_prefix_sum(__global const int *boundaries,
                                            __global int *_partial_prefix_sum,
                                            __global int *_offset_arr
                                            ) {

    uint mIndex, mIndexLast, mLastOffsetVal, mPrevIndexVal;
    uint mGlobalId = get_global_id(0);

    mIndex = mGlobalId * 32;
    mIndexLast = mIndex + 32;

    // Stores Partial Prefix Sum
    for(int i = mIndex; i < mIndexLast; i++) {
        mPrevIndexVal = (i == mIndex) ? 0 : _partial_prefix_sum[i - 1];
        _partial_prefix_sum[i] = (i == 0) ?
                    i : !(boundaries[i])  + mPrevIndexVal;
    }

    mLastOffsetVal = _partial_prefix_sum[mIndexLast - 1];

    // Stores the Offset Array Values
    for(int k = mGlobalId; k < get_global_size(0); k++) {
        atomic_add(&_offset_arr[k], mLastOffsetVal);
    }
}