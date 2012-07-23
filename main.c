#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<err.h>
#include<sys/mman.h>
#include<string.h>
#include<fcntl.h>
#define MEM_SIZE 30000
#define STACK_SIZE 1000
#define PROG_SIZE 65535

typedef struct {
  char* elements[STACK_SIZE];
  int top;
} stack;

void push(stack *s,char *element){
  if(s->top < STACK_SIZE){
    s->elements[s->top] = element;
    s->top++;
    return;
  }
  //Dont exactly know what this one does
  err(1,"STACK OVERFLOW");
}

char* pop(stack *s){
  if(s->top != 0){
    s->top--;
    return s->elements[s->top];
  }
  err(1,"STACK UNDERFLOW");
}

//Copied shamelessly from stack overflow. Need to see how to mmap this
char * read_program(char *prog){
  FILE *f = fopen(prog, "rb");
  fseek(f, 0, SEEK_END);
  long pos = ftell(f);
  fseek(f, 0, SEEK_SET);

  char *bytes = malloc(pos+1);
  fread(bytes, pos, 1, f);
  fclose(f);
  bytes[pos] = 0;
  return bytes;
}

int main(int argc, char* argv[]){
  char *p,*init_ptr;
  stack frame;
  char *instr_ptr;
  char *temp_iptr,*init_iptr;

  instr_ptr = init_iptr = read_program(argv[1]);
  frame.top = 0;
  p = init_ptr = (char*) malloc(sizeof(char)*MEM_SIZE);
  memset(p,0,MEM_SIZE);

  printf("==== Brain suck ===\n\n");

  while(*instr_ptr){
    //    printf("%d:%u:%c\n",*p,p,*instr_ptr);
    switch(*instr_ptr){
    case '+':
      (*p)++;
      instr_ptr++;
      break;
    case '-':
      (*p)--;
      instr_ptr++;
      break;
    case '>':
      p++;
      instr_ptr++;
      break;
    case '<':
      p--;
      instr_ptr++;
      break;
    case '.':
      putchar(*p);
      instr_ptr++;
      break;
    case ',':
      *p = getchar();
      instr_ptr++;
      break;
    case '[':
      push(&frame, instr_ptr);
      if(!*p){
	temp_iptr = instr_ptr;
	while(1){
	  instr_ptr++;
	  if(*instr_ptr == ']'){
	    char *last_brace = pop(&frame);
	    if(last_brace == temp_iptr){
	      break;
	    }
	  }else if(*instr_ptr == '['){
	    push(&frame,instr_ptr);
	  }
	}
      }
      instr_ptr++;
      break;
    case ']':
      instr_ptr = pop(&frame);
      break;
    default:
      instr_ptr++;
    }
  }
  free(init_iptr);
  free(init_ptr);
  return 0;

}
