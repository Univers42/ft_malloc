# *ALLOC

# ALLOCATION DYNAMIC UTILS

to start my own implementation of malloc and related functins, I shall begin by designing and coding my own allocatoor in a main source file.

The step I've chosen to follow are the followed :
1. implement the core allocator logic: manage a memory pool handling allocatin and freeing, and maintain free lists or blocks.
2. Then, I'll provide basic functions: malloc, free, realloc, calloc, memalign, valloc..
3. Next, I will add debugging, tracing, and statistics features as separate modules.

This project is about writing a dynamic allocation memory management library. So that we can use it with some programs already in use without modifying them or recompiling. This a long journey where each byte should be handled carefully by various type of headers. 

As it's a library itself, I've made the choice voluntarely not to implement he libft itself because it would have cause a `circular dependencie` that, according to me would have been ugly to manage. Besides that, it's more easy to maintain a full library that has no dependency. just rely on its own architecture.. For instance this project is easily edible to be added into my own libft a global context library that I continue to increment each time I feel the necessity.. 

> The reason why I did chose to do this project so early was because I was doing the minishell and thoughout my reserch, I hit a moment that I should create my own trackers of memory to for instance librate the error of childrent process needing liberation.


# So what's inside? 

There is three main function to reproduce, [re|m|]alloc and `free`... yes I was busy to write realloc and malloc,... yeah I just did that... anyway! 
Let's talk about those passionate functions, really used in modern programming in C.

so let's do a quick presentation:

## Malloc
malloc allocates the size bytes of memory and returns a pointer to the allocated memory. let's focus on that... It returns a pointer only because we don't care about the type ! we just want the address..

## Realloc
Realloc tries to chang ethe size of teh allocation pointed to by `ptr` to `size` and returns `ptr`. If there is not enough room to enlarge the memory alocaton pointed to by ptr, realloc creates a new allocation, copies as much of the old data pointed to by `ptr`as will fit to the new allocation, frees the old allocation and returns a pointer to the allocated memory.

for the old times sake <p style="color:red"></p>
```c
void    *realloc(void *ptr, size_t old, size_t new)
{
    void        *tmp;
    size_t      cp_size;

    if (new)
    {
        tmp = malloc(new);
        if (!tmp)
            return (NULL);
    }
    if (ptr && old)
    {
        cp_size = old < new ? old : new;
        memmove(tmp, ptr, cp_size);
        free(ptr);
    }
    return (tmp);
}

```
## Free