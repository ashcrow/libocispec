/* Copyright (C) 2017 Giuseppe Scrivano <giuseppe@scrivano.org>

libocispec is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

libocispec is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libocispec.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "config.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spec.h"

int
main (int argc, char *argv[])
{
  size_t rd;
  yajl_val tree;
  FILE *config;
  char *file_data = malloc (1024 * 1024);
  char errbuf[1024];

  printf("%s\n", getcwd(NULL,0));
  config = fopen ("tests/config.json", "r+");
  if (config == NULL)
    exit (1);
  rd = fread(file_data, 1, 1024 * 1024 - 1, config);
  if (rd < 0)
    exit (2);
  fclose (config);

  tree = yajl_tree_parse (file_data, errbuf, sizeof(errbuf));
  if (tree == NULL)
    exit (3);

  oci_parser_error err;
  oci_container_container *container = make_oci_container_container (tree, &err);

  if (container == NULL) {
    printf ("error %d\n", err);
    exit (4);
  }
  if (strcmp (container->hostname, "runc"))
    exit (5);
  if (strcmp (container->process->cwd, "/cwd"))
    exit (5);
  if (container->process->user->uid != 101)
    exit (5);
  if (strcmp (container->process->args[0], "ARGS1"))
    exit (5);
  if (strcmp (container->mounts[0]->destination, "/proc"))
    exit (5);

  free_oci_container_container (container);
  exit (0);
}
