﻿using System;
using Unity.Collections.LowLevel.Unsafe;
using Zoxel;

namespace Unity.Collections
{

    static public class NativeArrayExtensions
    {
        public static int IndexOf(this NativeArray<ZoxID> array, ZoxID value)
        {
            for (int i = 0; i < array.Length; i++)
            {
                if (array[i].id == value.id)
                {
                    return i;
                }
            }
            //UnityEngine.Debug.LogError("Could not find id");
            return -1;
        }
    }
        /*unsafe static public class NativeArrayExtensions
        {
            public static bool Contains<T, U>(this NativeArray<T> array, U value) where T : struct, IEquatable<U>
            {
                return IndexOf<T, U>(array.GetUnsafeReadOnlyPtr(), array.Length, value) != -1;
            }

            public static int IndexOf<T, U>(this NativeArray<T> array, U value) where T : struct, IEquatable<U>
            {
                return IndexOf<T, U>(array.GetUnsafeReadOnlyPtr(), array.Length, value);
            }

            public static bool Contains<T, U>(this NativeList<T> array, U value) where T : struct, IEquatable<U>
            {
                return IndexOf<T, U>(array.GetUnsafePtr(), array.Length, value) != -1;
            }

            public static int IndexOf<T, U>(this NativeList<T> array, U value) where T : struct, IEquatable<U>
            {
                return IndexOf<T, U>(array.GetUnsafePtr(), array.Length, value);
            }

            static int IndexOf<T, U>(void* ptr, int size, U value) where T : struct, IEquatable<U>
            {
                for (int i = 0; i != size; i++)
                {
                    if (UnsafeUtility.ReadArrayElement<T>(ptr, i).Equals(value))
                        return i;
                }
                return -1;
            }
        }*/
    }