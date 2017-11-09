#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;
  printf(1,"freeing memory at: %d", ap);
  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr){
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  }
  
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096){
    printf(1, "New memory request cannot be less than 4096 units. Setting to 4096\n");
    nu = 4096;
  }
  
  printf(1,"calling sbrk with the new memory request: %d plus the sizeof Header\n", nu);
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;
  printf(1, "malloc called with %d nbytes\n", nbytes);
  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    printf(1, "freelist is empty, initalize base and freep, prevp\n");
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  printf(1, "iterate through the freelist\n");
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits){
        printf(1, "block size is exactly the size of nunits\n");        
        prevp->s.ptr = p->s.ptr;
      }        
      else {
        printf(1, "blocksize is larger than nunits\n");
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
        printf(1, "set begining of space to %p\n", p);
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep){
      printf(1, "pointer is at the end of the list\n");
      if((p = morecore(nunits)) == 0){
        printf(1, "morecore could not get more memory from the OS\n");
        return 0;
      }
    }
      
  }
}
