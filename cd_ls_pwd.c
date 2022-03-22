/************* cd_ls_pwd.c file **************/
int cd()
{
  //printf("cd: under construction READ textbook!!!!\n");

  // READ Chapter 11.7.3 HOW TO chdir

  int ino = getino(pathname);
  MINODE *mip = iget(dev, ino);
  if(mip) // if a DIR
  {
    iput(running->cwd);
    running->cwd = mip;
    printf("cd: changed directories\n");
  }
  else{
    printf("cd: mip was not of type DIR\n");
  }
}

int ls_file(MINODE *mip, char *name)
{
  printf("ls_file: to be done: READ textbook!!!!\n");
  // READ Chapter 11.7.3 HOW TO ls
}

int ls_dir(MINODE *mip)
{
  printf("ls_dir: list CWD's file names; YOU FINISH IT as ls -l\n");

  char buf[BLKSIZE], temp[256];
  DIR *dp;
  char *cp;

  get_block(dev, mip->INODE.i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while (cp < buf + BLKSIZE){
     strncpy(temp, dp->name, dp->name_len);
     temp[dp->name_len] = 0;
	
     printf("%s  ", temp);

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}

int ls()
{
  // printf("ls: list CWD only! YOU FINISH IT for ls pathname\n");
  // ls_dir(running->cwd);
  int ino = getino(running->cwd);
  MINODE* mip = iget(dev, ino);

  int mode = mip->INODE.i_mode;

  if(S_ISDIR(mode))
    ls_dir(mip);
  else 
    ls_file(mip, running->cwd);

  iput(mip);
}

char *pwd(MINODE *wd)
{
  //printf("pwd: READ HOW TO pwd in textbook!!!!\n");
  if (wd == root){
    printf("/\n");
    return;
  }
  else{
    rpwd(wd);
  }
  //pwd start:
  pwd(running->cwd);

}

int rpwd(MINODE *wd)
{
  if(wd == root) return;
  int parent_ino = getino(&dev, running->cwd); // getting parent_ino
  MINODE* pip = iget(dev, parent_ino); // getting pip MINODE

  int my_ino = wd->INODE.i_block[0]; // getting my_ino

  pip->INODE.i_block[0]; // get my_name string by my_ino as Local
  rpwd(pip); // recursive call with parent minode
  //printf("/%s\n", my_name);
}

