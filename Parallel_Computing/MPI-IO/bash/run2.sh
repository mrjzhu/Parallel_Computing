/home/jzhu57/soft/mpich-install/bin/mpiexec -env LD_LIBRARY_PATH /home/jzhu57/soft/orangefs/lib:/home/jzhu57/soft/mpich-install/lib:$LD_LIBRARY_PATH -env PVFS2TAB_FILE /home/jzhu57/pvfs2tab -n $NSLOTS /home/jzhu57/run1/mpi_io_p2 pvfs2:/mnt/orangefs/file.txt 2 /home/jzhu57/run1/2MB.txt
